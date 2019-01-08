#pragma once

#include <string>

#include "io_binary_reader.h"

namespace mach::io{
	class binary_string_reader : public binary_reader{
	public:
		explicit binary_string_reader(const std::string &stream);

		explicit binary_string_reader(std::string &&stream);

		binary_string_reader(binary_string_reader &&reader);

		binary_string_reader &operator =(binary_string_reader &&reader);

		virtual std::size_t get_bytes_remaining() const override;

		virtual std::size_t read(byte *buffer, std::size_t size) override;

	private:
		std::string stream_;
		std::size_t offset_ = 0u;
	};
}
