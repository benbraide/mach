#pragma once

#include <cstdlib>

namespace mach::io{
	class binary_reader{
	public:
		using byte = unsigned __int8;

		virtual std::size_t get_bytes_remaining() const = 0;

		virtual std::size_t read(byte *buffer, std::size_t size) = 0;

		template <typename target_type>
		target_type read_scalar(){
			auto buffer = target_type();
			return ((read(reinterpret_cast<byte *>(&buffer), sizeof(target_type)) == sizeof(target_type)) ? buffer : target_type());
		}

		template <typename target_type>
		std::size_t read_buffer(target_type *buffer, std::size_t size){
			return read(reinterpret_cast<byte *>(buffer), (sizeof(target_type) * size));
		}
	};
}
