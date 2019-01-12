#include "machine_memory_block.h"

mach::machine::memory_block::memory_block(qword address, std::size_t size, unsigned int attributes)
	: address_(address), attributes_(attributes), size_(size), real_size_((size == 0u) ? 1u : size){
	data_ = std::make_unique<byte[]>(real_size_);
}

mach::machine::memory_block::~memory_block(){
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

mach::machine::memory_block::qword mach::machine::memory_block::get_address() const{
	return address_;
}

mach::machine::memory_block::qword mach::machine::memory_block::get_address_offset(qword address) const{
	return ((address_ <= address) ? (address - address_) : static_cast<qword>(-1));
}

bool mach::machine::memory_block::address_is_inside_block(qword address) const{
	return (address_ <= address && (address - address_) < get_real_size());
}

bool mach::machine::memory_block::address_range_intersects_block(qword first, qword last) const{
	return (address_is_inside_block(first) || address_is_inside_block(last));
}

std::size_t mach::machine::memory_block::get_size() const{
	return size_;
}

std::size_t mach::machine::memory_block::get_real_size() const{
	return real_size_;
}

mach::machine::memory_block::byte *mach::machine::memory_block::get_data() const{
	return data_.get();
}

unsigned int mach::machine::memory_block::get_attributes() const{
	return attributes_;
}

std::size_t mach::machine::memory_block::read(qword offset, byte *buffer, std::size_t size) const{
	if ((attributes_ & access_protected) != 0u)//Access protected
		throw memory_error_code::access_protected;

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	auto ptr = data_.get();
	if (0u < size && size <= real_size_)
		memcpy(buffer, (data_.get() + offset), size);

	return ((size <= real_size_) ? size : 0u);
}

std::size_t mach::machine::memory_block::write(qword offset, const byte *buffer, std::size_t size){
	if ((attributes_ & write_protected) != 0u)//Write protected
		throw memory_error_code::write_protected;

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	auto ptr = data_.get();
	if (0u < size && size <= real_size_)
		memcpy((data_.get() + offset), buffer, size);

	return ((size <= real_size_) ? size : 0u);
}

std::size_t mach::machine::memory_block::set(qword offset, byte value, std::size_t size){
	if ((attributes_ & write_protected) != 0u)//Write protected
		throw memory_error_code::write_protected;

	if ((real_size_ - offset) < size)//Restrict size
		size = (real_size_ - offset);

	if (0u < size && size <= real_size_)
		memset((data_.get() + offset), value, size);

	return ((size <= real_size_) ? size : 0u);
}

bool mach::machine::memory_block::offset_is_inside_block_(qword offset) const{
	return (offset < real_size_);
}

void mach::machine::memory_block::set_previous_block_(memory_block *block){
	block->next_block_ = this;
	previous_block_ = block;
}

void mach::machine::memory_block::set_next_block_(memory_block *block){
	block->previous_block_ = this;
	next_block_ = block;
}
