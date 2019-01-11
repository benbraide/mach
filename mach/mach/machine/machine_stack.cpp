#include "machine_stack.h"

mach::machine::stack::stack(byte *buffer, std::size_t size)
	: buffer_(buffer), size_(size){}

std::size_t mach::machine::stack::push(const byte *buffer, std::size_t size){
	if ((size_ - offset_) < size)//Restrict size
		size = (size_ - offset_);

	if (0u < size){//Copy if necessary
		memcpy((buffer_ + offset_), buffer, size);
		offset_ += size;
	}

	return size;
}

bool mach::machine::stack::push(const register_object &reg){
	return (push(reg.get_data(), reg.get_size()) == reg.get_size());
}

std::size_t mach::machine::stack::pop(byte *buffer, std::size_t size){
	if (offset_ < size)//Restrict size
		size = offset_;

	if (0u < size){//Copy if necessary
		offset_ -= size;
		memcpy(buffer, (buffer_ + offset_), size);
	}

	return size;
}

bool mach::machine::stack::pop(register_object &reg){
	return (pop(reg.get_data(), reg.get_size()) == reg.get_size());
}

std::size_t mach::machine::stack::get_offset() const{
	return offset_;
}
