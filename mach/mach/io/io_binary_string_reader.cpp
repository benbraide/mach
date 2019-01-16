#include "io_binary_string_reader.h"

mach::io::binary_string_reader::binary_string_reader(const std::string &stream)
	: stream_(stream){}

mach::io::binary_string_reader::binary_string_reader(std::string &&stream)
	: stream_(std::move(stream)){}

mach::io::binary_string_reader::binary_string_reader(binary_string_reader &&reader)
	: stream_(std::move(reader.stream_)), offset_(reader.offset_){
	reader.offset_ = 0;
}

mach::io::binary_string_reader &mach::io::binary_string_reader::operator=(binary_string_reader &&reader){
	stream_ = std::move(reader.stream_);
	offset_ = reader.offset_;

	reader.offset_ = 0u;
	return *this;
}

std::size_t mach::io::binary_string_reader::get_offset() const{
	return offset_;
}

std::size_t mach::io::binary_string_reader::get_bytes_remaining() const{
	return (stream_.size() - offset_);
}

std::size_t mach::io::binary_string_reader::ignore(std::size_t size) const{
	if ((stream_.size() - offset_) < size)
		size = (stream_.size() - offset_);//Adjust size

	offset_ += size;
	return size;
}

std::size_t mach::io::binary_string_reader::read(byte *buffer, std::size_t size) const{
	if ((stream_.size() - offset_) < size)
		size = (stream_.size() - offset_);//Adjust size

	if (0u < size){//Valid size
		memcpy(buffer, (stream_.data() + offset_), size);
		offset_ += size;
	}

	return size;
}
