#pragma once

#include "../io/io_binary_buffer_reader.h"
#include "../byte_code/byte_code_instruction.h"

namespace mach::machine{
	class runtime{
	public:
		using byte = byte_code::instruction::byte;
		using qword = byte_code::instruction::qword;

		explicit runtime(io::reader &reader);

		explicit runtime(qword entry);

		runtime(byte *data, std::size_t size);

		void run();

	private:
		void init_(io::reader &reader);

		void init_(byte *data, std::size_t size);

		void prepare_(io::reader &reader);

		register_table reg_table_;
		std::shared_ptr<stack> stack_;

		byte *data_ = nullptr;
		io::reader *reader_ = nullptr;

		std::size_t module_size_ = 0u;
		qword entry_ = 0u;

		static memory memory_;
		static std::size_t stack_size_;
	};
}
