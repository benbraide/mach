#include "io_binary_file_reader.h"

mach::io::binary_file_reader::binary_file_reader(const std::string &stream)
	: stream_(stream.data(), std::ios::binary){
	if (stream_.is_open()){//Compute size
		auto end = stream_.seekg(0, std::ios::end).tellg();
		auto begin = stream_.seekg(0, std::ios::beg).tellg();
		size_ = static_cast<std::size_t>(end - begin);
	}
}

mach::io::binary_file_reader::binary_file_reader(binary_file_reader &&reader)
	: stream_(std::move(reader.stream_)), size_(reader.size_), offset_(reader.offset_){
	reader.size_ = 0u;
	reader.offset_ = 0;
}

mach::io::binary_file_reader &mach::io::binary_file_reader::operator=(binary_file_reader &&reader){
	stream_ = std::move(reader.stream_);
	size_ = reader.size_;
	offset_ = reader.offset_;

	reader.size_ = 0u;
	reader.offset_ = 0;

	return *this;
}

std::size_t mach::io::binary_file_reader::get_bytes_remaining() const{
	return (size_ - offset_);
}

std::size_t mach::io::binary_file_reader::read(byte *buffer, std::size_t size){
	if (auto remaining_ = (size_ - offset_); remaining_ < size)
		size = remaining_;//Adjust size

	if (0u < size){//Valid size
		stream_.read(reinterpret_cast<char *>(buffer), size);
		offset_ += size;
	}

	return size;
}
