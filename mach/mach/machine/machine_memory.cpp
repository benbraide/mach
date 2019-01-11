#include "machine_memory.h"

mach::machine::memory::memory(){
	allocate_block_(0u, (memory_block::access_protected | memory_block::write_protected));
}

std::size_t mach::machine::memory::read(qword address, byte *buffer, std::size_t size, unsigned int *exception) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return read_(address, buffer, size, exception);
}

std::size_t mach::machine::memory::write(qword address, const byte *buffer, std::size_t size, unsigned int *exception){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return write_(address, buffer, size, true, exception);
}

std::size_t mach::machine::memory::set(qword address, byte value, std::size_t size, unsigned int *exception){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return write_(address, &value, size, false, exception);
}

std::size_t mach::machine::memory::copy(qword source, qword destination, std::size_t size, unsigned int *exception){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return copy_(source, destination, size, exception);
}

std::size_t mach::machine::memory::copy(const io::reader &source, qword destination, std::size_t size, unsigned int *exception){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return copy_(source, destination, size, exception);
}

std::size_t mach::machine::memory::copy(qword source, io::writer &destination, std::size_t size, unsigned int *exception){
	std::shared_lock<std::shared_mutex> guard(lock_);
	return copy_(source, destination, size, exception);
}

mach::machine::memory_block *mach::machine::memory::allocate_block(std::size_t size, unsigned int attributes){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_block_(size, attributes);
}

mach::machine::memory_block *mach::machine::memory::allocate_partial_block(qword address, qword offset, std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_partial_block_(address, offset, size);
}

mach::machine::memory_block *mach::machine::memory::allocate_partial_block(full_memory_block &block, qword offset, std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return allocate_partial_block_(block, offset, size);
}

mach::machine::memory_block *mach::machine::memory::reallocate_block(qword address, std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return reallocate_block_(address, size);
}

mach::machine::memory_block *mach::machine::memory::reallocate_block(full_memory_block &block, std::size_t size){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return reallocate_block_(block, size);
}

bool mach::machine::memory::deallocate_block(qword address){
	std::lock_guard<std::shared_mutex> guard(lock_);
	return deallocate_block_(address);
}

mach::machine::full_memory_block *mach::machine::memory::find_block(qword address) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return find_block_(address);
}

mach::machine::full_memory_block *mach::machine::memory::find_block_containing_address(qword address) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return find_block_containing_address_(address);
}

void mach::machine::memory::set_range_attributes(qword start, qword end, unsigned int attributes){
	std::lock_guard<std::shared_mutex> guard(lock_);
	set_range_attributes_(start, end, attributes);
}

unsigned int mach::machine::memory::get_range_attributes(qword start, qword end) const{
	std::shared_lock<std::shared_mutex> guard(lock_);
	return get_range_attributes_(start, end);
}

bool mach::machine::memory::address_is_inside_range(qword address, qword start, qword end){
	return (start <= address && address <= end);
}

std::size_t mach::machine::memory::read_(qword address, byte *buffer, std::size_t size, unsigned int *exception) const{
	if (auto range_attributes = get_range_attributes_(address, (address + size)); (range_attributes & memory_block::access_protected) != 0u){
		if (exception != nullptr)
			*exception = memory_block::access_protected;
		return 0u;
	}

	std::size_t bytes_read = 0u, read_count = 0u;
	for (auto block = find_block_containing_address_(address); bytes_read < size; block = dynamic_cast<full_memory_block *>(block->next_block_)){
		if (block == nullptr){//Access violation
			if (exception != nullptr)
				*exception = memory_block::access_protected;
			break;
		}

		read_count = block->read(block->get_address_offset(address), (buffer + bytes_read), (size - bytes_read), exception);
		bytes_read += read_count;
		address += read_count;

		if (exception != nullptr && *exception != 0u)
			break;//Exception thrown
	}

	return bytes_read;
}

std::size_t mach::machine::memory::write_(qword address, const byte *buffer, std::size_t size, bool is_array_buffer, unsigned int *exception){
	if (auto range_attributes = get_range_attributes_(address, (address + size)); (range_attributes & memory_block::write_protected) != 0u){
		if (exception != nullptr)
			*exception = memory_block::write_protected;
		return 0u;
	}

	std::size_t bytes_written = 0u, write_count = 0u;
	for (auto block = find_block_containing_address_(address); bytes_written < size ; block = dynamic_cast<full_memory_block *>(block->next_block_)){
		if (block == nullptr){//Access violation
			if (exception != nullptr)
				*exception = memory_block::access_protected;
			break;
		}

		if (is_array_buffer)
			write_count = block->write(block->get_address_offset(address), (buffer + bytes_written), (size - bytes_written), exception);
		else//Set
			write_count = block->set(block->get_address_offset(address), *buffer, (size - bytes_written), exception);

		bytes_written += write_count;
		address += write_count;

		if (exception != nullptr && *exception != 0u)
			break;//Exception thrown
	}

	return bytes_written;
}

std::size_t mach::machine::memory::copy_(qword source, qword destination, std::size_t size, unsigned int *exception){
	if (auto range_attributes = get_range_attributes_(source, (source + size)); (range_attributes & memory_block::access_protected) != 0u){
		if (exception != nullptr)
			*exception = memory_block::access_protected;
		return 0u;
	}

	if (auto range_attributes = get_range_attributes_(destination, (destination + size)); (range_attributes & memory_block::write_protected) != 0u){
		if (exception != nullptr)
			*exception = memory_block::write_protected;
		return 0u;
	}

	auto source_block = find_block_containing_address_(source);
	auto destination_block = find_block_containing_address_(destination);

	std::size_t bytes_copied = 0u, copy_count = 0u;
	while (bytes_copied < size){
		if (source_block == nullptr || destination_block == nullptr){//Access violation
			if (exception != nullptr)
				*exception = memory_block::access_protected;
			break;
		}

		auto source_offset = source_block->get_address_offset(source), destination_offset = destination_block->get_address_offset(destination);
		auto source_size = (source_block->real_size_ - source_offset);

		if (source_size < (copy_count = (size - bytes_copied)))
			copy_count = source_size;//Restrict size

		if ((source_block->attributes_ & memory_block::access_protected) != 0u){
			if (exception != nullptr)
				*exception = memory_block::access_protected;
			break;
		}

		copy_count = destination_block->write(destination_offset, (source_block->data_.get() + source_offset), copy_count, exception);
		bytes_copied += copy_count;

		source += copy_count;
		destination += copy_count;

		if (exception != nullptr && *exception != 0u)
			break;//Exception thrown

		if ((source_block->real_size_ - source_offset) == copy_count)//End of block
			source_block = dynamic_cast<full_memory_block *>(source_block->next_block_);

		if ((destination_block->real_size_ - destination_offset) == copy_count)//End of block
			destination_block = dynamic_cast<full_memory_block *>(destination_block->next_block_);
	}

	return bytes_copied;
}

std::size_t mach::machine::memory::copy_(const io::reader &source, qword destination, std::size_t size, unsigned int *exception){
	if (auto range_attributes = get_range_attributes_(destination, (destination + size)); (range_attributes & memory_block::write_protected) != 0u){
		if (exception != nullptr)
			*exception = memory_block::write_protected;
		return 0u;
	}

	std::size_t bytes_copied = 0u, copy_count = 0u;
	for (auto block = find_block_containing_address_(destination); bytes_copied < size; block = dynamic_cast<full_memory_block *>(block->next_block_)){
		if (block == nullptr){//Access violation
			if (exception != nullptr)
				*exception = memory_block::access_protected;
			break;
		}

		if ((block->attributes_ & memory_block::write_protected) != 0u){
			if (exception != nullptr)
				*exception = memory_block::write_protected;
			break;
		}

		auto destination_offset = block->get_address_offset(destination);
		auto destination_size = (block->real_size_ - destination_offset);

		if (destination_size < (copy_count = (size - bytes_copied)))
			copy_count = destination_size;//Restrict size

		copy_count = source.read((block->data_.get() + destination_offset), destination_size);
		bytes_copied += copy_count;
		destination += copy_count;

		if (copy_count != destination_size || (exception != nullptr && *exception != 0u))
			break;//Incomplete read OR Exception thrown
	}

	return bytes_copied;
}

std::size_t mach::machine::memory::copy_(qword source, io::writer &destination, std::size_t size, unsigned int *exception){
	if (auto range_attributes = get_range_attributes_(source, (source + size)); (range_attributes & memory_block::write_protected) != 0u){
		if (exception != nullptr)
			*exception = memory_block::write_protected;
		return 0u;
	}

	std::size_t bytes_copied = 0u, copy_count = 0u;
	for (auto block = find_block_containing_address_(source); bytes_copied < size; block = dynamic_cast<full_memory_block *>(block->next_block_)){
		if (block == nullptr || (block->attributes_ & memory_block::access_protected) != 0u){//Access violation
			if (exception != nullptr)
				*exception = memory_block::access_protected;
			break;
		}

		auto source_offset = block->get_address_offset(source);
		auto source_size = (block->real_size_ - source_offset);

		if (source_size < (copy_count = (size - bytes_copied)))
			copy_count = source_size;//Restrict size

		copy_count = destination.write((block->data_.get() + source_offset), source_size);
		bytes_copied += copy_count;
		source += copy_count;

		if (copy_count != source_size || (exception != nullptr && *exception != 0u))
			break;//Incomplete write OR Exception thrown
	}

	return bytes_copied;
}

mach::machine::memory_block *mach::machine::memory::allocate_block_(std::size_t size, unsigned int attributes){
	auto block = std::make_shared<full_memory_block>(0u, size, attributes);
	if (block == nullptr)//Failed to allocate memory
		return nullptr;

	auto address = extract_free_space_(block->real_size_);
	if (address == static_cast<qword>(-1)){//Use next address
		available_sizes_[(address = next_address_)] = size_info{ block->real_size_, false };
		next_address_ += block->real_size_;
	}

	block->address_ = address;
	blocks_[address] = block;

	if (auto prev_block = find_block_containing_address_(address - 1u); prev_block != nullptr)
		block->set_previous_block_(prev_block);

	if (auto next_block = find_block_containing_address_(address + block->real_size_); next_block != nullptr)
		block->set_next_block_(next_block);

	return block.get();
}

mach::machine::memory_block *mach::machine::memory::allocate_partial_block_(qword address, qword offset, std::size_t size){
	auto block = find_block_(address);
	return ((block == nullptr) ? nullptr : allocate_partial_block_(*block, offset, size));
}

mach::machine::memory_block *mach::machine::memory::allocate_partial_block_(full_memory_block &block, qword offset, std::size_t size){
	return block.insert_partial_block_(offset, size);
}

mach::machine::memory_block *mach::machine::memory::reallocate_block_(qword address, std::size_t size){
	auto block = find_block_(address);
	return ((block == nullptr) ? nullptr : reallocate_block_(*block, size));
}

mach::machine::memory_block *mach::machine::memory::reallocate_block_(full_memory_block &block, std::size_t size){
	if (block.real_size_ < size){//Extend
		if (auto free_entry = available_sizes_.find(block.address_ + block.real_size_); free_entry != available_sizes_.end()){
			if (auto attributes = block.attributes_; !free_entry->second.is_free || free_entry->second.value < size){
				deallocate_block_(block);
				return allocate_block_(size, attributes);//Cannot accommodate size
			}

			std::prev(free_entry)->second.value = size;
			if (size < free_entry->second.value)//Split entry
				available_sizes_[(free_entry->first + size)] = size_info{ (free_entry->second.value - size) };

			available_sizes_.erase(free_entry);//Erase consumed entry
		}
		else{//Extend with next address
			available_sizes_.rbegin()->second.value = size;
			next_address_ += (size - block.real_size_);
		}

		if (size != 0u)//Update values
			block.real_size_ = block.size_ = size;
	}
	else if (size < block.real_size_)//Real size accommodates size
		block.size_ = size;

	return &block;
}

bool mach::machine::memory::deallocate_block_(qword address){
	auto it = blocks_.find(address);
	if (it == blocks_.end() || !add_free_space_(address))
		return false;//Block not found OR error

	blocks_.erase(it);
	return true;
}

bool mach::machine::memory::deallocate_block_(full_memory_block &block){
	return deallocate_block_(block.address_);
}

bool mach::machine::memory::add_free_space_(qword address){
	auto it = available_sizes_.find(address);
	if (it == available_sizes_.end())
		return false;

	it->second.is_free = true;
	if (auto prev_entry = ((it == available_sizes_.begin()) ? available_sizes_.end() : std::prev(it)); prev_entry != available_sizes_.end() && prev_entry->second.is_free){//Merge with previous
		prev_entry->second.value += it->second.value;
		available_sizes_.erase(it);
		it = prev_entry;
	}

	if (auto next_entry = std::next(it); next_entry != available_sizes_.end() && next_entry->second.is_free){//Merge with next
		it->second.value += next_entry->second.value;
		available_sizes_.erase(next_entry);
	}

	return true;
}

mach::machine::memory::qword mach::machine::memory::extract_free_space_(std::size_t size){
	auto address = static_cast<qword>(-1);
	for (auto it = available_sizes_.begin(); it != available_sizes_.end(); ++it){
		if (!it->second.is_free || it->second.value < size)
			continue;//Cannot accommodate size

		address = it->first;
		it->second.is_free = false;

		if (auto entry_size = it->second.value; entry_size != size){//Use required
			it->second.value = size;
			available_sizes_[(address + size)] = size_info{ (entry_size - size) };
		}

		break;
	}

	return address;
}

mach::machine::full_memory_block *mach::machine::memory::find_block_(qword address) const{
	auto it = blocks_.find(address);
	return ((it == blocks_.end()) ? nullptr : it->second.get());
}

mach::machine::full_memory_block *mach::machine::memory::find_block_containing_address_(qword address) const{
	if (blocks_.empty())
		return nullptr;

	for (auto &block : blocks_){
		if (block.second->address_is_inside_block(address))
			return block.second.get();

		if (address < block.first)
			break;//No match can be found
	}

	return nullptr;
}

void mach::machine::memory::set_range_attributes_(qword start, qword end, unsigned int attributes){
	range_attributes_.push_back(range_info{ start, end, attributes });
}

unsigned int mach::machine::memory::get_range_attributes_(qword start, qword end) const{
	unsigned int attributes = 0u;
	for (auto &info : range_attributes_){
		if (address_is_inside_range(start, info.start, info.end) || address_is_inside_range(end, info.start, info.end))
			attributes |= info.attributes;
	}

	return attributes;
}
