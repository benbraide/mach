#pragma once

#include <cstdlib>

namespace mach::io{
	class writer{
	public:
		using byte = unsigned __int8;

		virtual ~writer() = default;

		virtual std::size_t write(const byte *buffer, std::size_t size) = 0;

		template <typename target_type>
		bool write_scalar(target_type buffer){
			return (write(reinterpret_cast<const byte *>(&buffer), sizeof(target_type)) == sizeof(target_type));
		}

		template <typename target_type>
		std::size_t write_buffer(const target_type *buffer, std::size_t size){
			return write(reinterpret_cast<const byte *>(buffer), (sizeof(target_type) * size));
		}
	};
}
