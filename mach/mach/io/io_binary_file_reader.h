#pragma once

#include <string>
#include <fstream>

#include "io_binary_reader.h"

namespace mach::io{
	class binary_file_reader : public binary_reader{
	public:
		explicit binary_file_reader(const std::string &stream);

		binary_file_reader(binary_file_reader &&reader);

		binary_file_reader &operator =(binary_file_reader &&reader);

		virtual std::size_t get_bytes_remaining() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

	private:
		mutable std::ifstream stream_;
		std::size_t size_ = 0u;
		mutable std::size_t offset_ = 0u;
	};
}
