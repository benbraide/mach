#include "machine_memory_block.h"

mach::machine::memory_block::~memory_block() = default;

mach::machine::memory_block::qword mach::machine::memory_block::get_address_offset(qword address) const{
	auto this_address = get_address();
	return ((this_address <= address) ? (address - this_address) : static_cast<qword>(-1));
}

bool mach::machine::memory_block::address_is_inside_block(qword address) const{
	auto this_address = get_address();
	return (this_address <= address && address < (this_address + get_real_size()));
}

bool mach::machine::memory_block::address_range_intersects_block(qword first, qword last) const{
	return (address_is_inside_block(first) || address_is_inside_block(last));
}

bool mach::machine::memory_block::offset_is_inside_block_(qword offset) const{
	return (offset < get_real_size());
}

void mach::machine::memory_block::set_previous_block_(memory_block *block){
	block->next_block_ = this;
	previous_block_ = block;
}

void mach::machine::memory_block::set_next_block_(memory_block *block){
	block->previous_block_ = this;
	next_block_ = block;
}

mach::machine::full_memory_block::full_memory_block(qword address, std::size_t size, unsigned int attributes)
	: address_(address), attributes_(attributes), size_(size), real_size_((size == 0u) ? 1u : size){
	data_ = std::make_unique<byte[]>(real_size_);
}

mach::machine::full_memory_block::~full_memory_block(){
	if (previous_block_ != nullptr){//Remove association
		if (previous_block_->next_block_ == this)
			previous_block_->next_block_ = nullptr;
		previous_block_ = nullptr;
	}

	if (next_block_ != nullptr){//Remove association
		if (next_block_->previous_block_ == this)
			next_block_->previous_block_ = nullptr;
		next_block_ = nullptr;
	}
}

mach::machine::memory_block::qword mach::machine::full_memory_block::get_address() const{
	return address_;
}

std::size_t mach::machine::full_memory_block::get_size() const{
	return size_;
}

std::size_t mach::machine::full_memory_block::get_real_size() const{
	return real_size_;
}

mach::machine::memory_block::byte *mach::machine::full_memory_block::get_data() const{
	return data_.get();
}

unsigned int mach::machine::full_memory_block::get_attributes() const{
	return attributes_;
}

std::size_t mach::machine::full_memory_block::read(qword offset, byte *buffer, std::size_t size, unsigned int *exception) const{
	if ((attributes_ & access_protected) != 0u){//Access protected
		if (exception != nullptr)
			*exception = access_protected;

		return 0u;
	}

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	auto ptr = data_.get();
	if (0u < size && size <= real_size_)
		memcpy(buffer, (data_.get() + offset), size);

	return ((size <= real_size_) ? size : 0u);
}

std::size_t mach::machine::full_memory_block::write(qword offset, const byte *buffer, std::size_t size, unsigned int *exception){
	if ((attributes_ & write_protected) != 0u){//Write protected
		if (exception != nullptr)
			*exception = write_protected;

		return 0u;
	}

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	auto ptr = data_.get();
	if (0u < size && size <= real_size_)
		memcpy((data_.get() + offset), buffer, size);

	return ((size <= real_size_) ? size : 0u);
}

std::size_t mach::machine::full_memory_block::set(qword offset, byte value, std::size_t size, unsigned int *exception){
	if ((attributes_ & write_protected) != 0u){//Write protected
		if (exception != nullptr)
			*exception = write_protected;

		return 0u;
	}

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	if (0u < size && size <= real_size_)
		memset((data_.get() + offset), value, size);

	return ((size <= real_size_) ? size : 0u);
}

mach::machine::memory_block *mach::machine::full_memory_block::insert_partial_block_(qword offset, std::size_t size){
	if (find_partial_block_(offset) != nullptr)
		return nullptr;//Partial block already exists at offset

	auto partial_block = std::make_shared<partial_memory_block>(*this, offset, size);
	if (partial_block == nullptr)//Failed to allocate memory
		return nullptr;

	partial_blocks_[offset] = partial_block;
	if (offset != 0u){//Find previous block
		if (auto block = find_partial_block_containing_offset_(offset - 1u); block != nullptr){//Block found
			block->next_block_ = partial_block.get();
			partial_block->previous_block_ = block;
		}
	}

	if ((offset + partial_block->real_size_) < real_size_){//Find next block
		if (auto block = find_partial_block_containing_offset_(offset + partial_block->real_size_); block != nullptr){//Block found
			block->previous_block_ = partial_block.get();
			partial_block->next_block_ = block;
		}
	}

	return partial_block.get();
}

mach::machine::memory_block *mach::machine::full_memory_block::find_partial_block_(qword offset) const{
	auto it = partial_blocks_.find(offset);
	return ((it == partial_blocks_.end()) ? nullptr : it->second.get());
}

mach::machine::memory_block *mach::machine::full_memory_block::find_partial_block_containing_offset_(qword offset) const{
	if (partial_blocks_.empty())
		return nullptr;

	for (auto &block : partial_blocks_){
		if (block.second->offset_is_inside_block_(offset))
			return block.second.get();

		if (offset < block.first)
			break;//No match can be found
	}

	return nullptr;
}

mach::machine::memory_block *mach::machine::full_memory_block::find_next_partial_block_(qword offset) const{
	if (partial_blocks_.empty())
		return nullptr;

	for (auto &block : partial_blocks_){
		if (offset < block.first)
			return block.second.get();
	}

	return nullptr;
}

mach::machine::partial_memory_block::partial_memory_block(memory_block &block, qword offset, std::size_t size)
	: block_(block), offset_(offset), size_(size), real_size_((size == 0u) ? 1u : size){}

mach::machine::partial_memory_block::~partial_memory_block(){
	if (previous_block_ != nullptr){//Remove association
		if (previous_block_->next_block_ == this)
			previous_block_->next_block_ = nullptr;
		previous_block_ = nullptr;
	}

	if (next_block_ != nullptr){//Remove association
		if (next_block_->previous_block_ == this)
			next_block_->previous_block_ = nullptr;
		next_block_ = nullptr;
	}
}

mach::machine::memory_block::qword mach::machine::partial_memory_block::get_address() const{
	return (block_.get_address() + offset_);
}

std::size_t mach::machine::partial_memory_block::get_size() const{
	return size_;
}

std::size_t mach::machine::partial_memory_block::get_real_size() const{
	return real_size_;
}

mach::machine::memory_block::byte *mach::machine::partial_memory_block::get_data() const{
	return (block_.get_data() + offset_);
}

unsigned int mach::machine::partial_memory_block::get_attributes() const{
	return block_.get_attributes();
}

std::size_t mach::machine::partial_memory_block::read(qword offset, byte *buffer, std::size_t size, unsigned int *exception) const{
	if ((get_attributes() & access_protected) != 0u){//Access protected
		if (exception != nullptr)
			*exception = access_protected;

		return 0u;
	}

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	if (0u < size && size <= real_size_)
		memcpy(buffer, (get_data() + offset), size);

	return ((size <= real_size_) ? size : 0u);
}

std::size_t mach::machine::partial_memory_block::write(qword offset, const byte *buffer, std::size_t size, unsigned int *exception){
	if ((get_attributes() & write_protected) != 0u){//Write protected
		if (exception != nullptr)
			*exception = write_protected;

		return 0u;
	}

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	if (0u < size && size <= real_size_)
		memcpy((get_data() + offset), buffer, size);

	return ((size <= real_size_) ? size : 0u);
}

std::size_t mach::machine::partial_memory_block::set(qword offset, byte value, std::size_t size, unsigned int *exception){
	if ((get_attributes() & write_protected) != 0u){//Write protected
		if (exception != nullptr)
			*exception = write_protected;

		return 0u;
	}

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	if (0u < size && size <= real_size_)
		memset((get_data() + offset), value, size);

	return ((size <= real_size_) ? size : 0u);
}
