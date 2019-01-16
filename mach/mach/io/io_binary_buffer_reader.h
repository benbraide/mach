#pragma once

#include <string>

#include "io_binary_reader.h"

namespace mach::io{
	class binary_buffer_reader : public binary_reader{
	public:
		binary_buffer_reader(byte *buffer, std::size_t size, std::size_t offset = 0u);

		binary_buffer_reader(binary_buffer_reader &&reader);

		binary_buffer_reader &operator =(binary_buffer_reader &&reader);

		virtual std::size_t get_offset() const override;

		virtual std::size_t get_bytes_remaining() const override;

		virtual std::size_t ignore(std::size_t size) const override;

		virtual std::size_t read(byte *buffer, std::size_t size) const override;

	private:
		byte *buffer_;
		std::size_t size_ = 0u;
		mutable std::size_t offset_ = 0u;
	};
}
