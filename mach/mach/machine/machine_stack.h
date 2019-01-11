#pragma once

#include "machine_register.h"

namespace mach::machine{
	class stack{
	public:
		using byte = register_object::byte;

		stack(byte *buffer, std::size_t size);

		std::size_t push(const byte *buffer, std::size_t size);

		bool push(const register_object &reg);

		std::size_t pop(byte *buffer, std::size_t size);

		bool pop(register_object &reg);

		std::size_t get_offset() const;

	private:
		byte *buffer_ = nullptr;
		std::size_t size_ = 0u;
		std::size_t offset_ = 0u;
	};
}
