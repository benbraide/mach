#include "io_binary_buffer_reader.h"

mach::io::binary_buffer_reader::binary_buffer_reader(byte *buffer, std::size_t size, std::size_t offset)
	: buffer_(buffer), size_(size), offset_(offset){}

mach::io::binary_buffer_reader::binary_buffer_reader(binary_buffer_reader &&reader)
	: buffer_(reader.buffer_), size_(reader.size_), offset_(reader.offset_){
	reader.buffer_ = nullptr;
	reader.size_ = 0u;
	reader.offset_ = 0;
}

mach::io::binary_buffer_reader &mach::io::binary_buffer_reader::operator=(binary_buffer_reader &&reader){
	buffer_ = reader.buffer_;
	reader.buffer_ = nullptr;


	size_ = reader.size_;
	offset_ = reader.offset_;

	reader.size_ = 0u;
	reader.offset_ = 0;

	return *this;
}

std::size_t mach::io::binary_buffer_reader::get_offset() const{
	return offset_;
}

std::size_t mach::io::binary_buffer_reader::get_bytes_remaining() const{
	return (size_ - offset_);
}

std::size_t mach::io::binary_buffer_reader::ignore(std::size_t size) const{
	if ((size_ - offset_) < size)
		size = (size_ - offset_);//Adjust size

	offset_ += size;
	return size;
}

std::size_t mach::io::binary_buffer_reader::read(byte *buffer, std::size_t size) const{
	if ((size_ - offset_) < size)
		size = (size_ - offset_);//Adjust size

	if (0u < size){//Valid size
		memcpy(buffer, (buffer_ + offset_), size);
		offset_ += size;
	}

	return size;
}
