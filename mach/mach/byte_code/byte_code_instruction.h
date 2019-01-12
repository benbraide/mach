#pragma once

#include <variant>

#include "../machine/machine_stack.h"
#include "../machine/machine_memory.h"
#include "../machine/machine_op_code.h"
#include "../machine/machine_register_table.h"

namespace mach::byte_code{
	class instruction{
	public:
		using byte = machine::memory::byte;
		using qword = machine::memory::qword;
		using operand_type = std::variant<std::nullptr_t, machine::register_object *, byte *, qword>;

		virtual unsigned int execute(machine::memory &memory, machine::register_table &reg_table, machine::stack &stack) const;

		static machine::register_object *get_register(std::size_t size, std::size_t index, machine::register_table &reg_table);

		static operand_type extract_operand(std::size_t size, machine::memory &memory, machine::register_table &reg_table, bool write_to_first_temp_buffer = true);

		static byte *extract_data(std::size_t size, machine::memory &memory, machine::register_table &reg_table, bool write_to_first_temp_buffer = true);

		template <typename target_type>
		static target_type extract_value(machine::memory &memory, machine::register_table &reg_table, unsigned int *exception = nullptr){
			auto data = extract_data(sizeof(target_type), memory, reg_table, false);
			if (data == nullptr){//Error
				if (exception != nullptr)
					*exception = execution_code_access_protected;
				return target_type();
			}

			return convert_data_to_value<target_type>(data);
		}

		template <typename target_type>
		static target_type extract_offset(std::size_t count, machine::memory &memory, machine::register_table &reg_table, unsigned int *exception = nullptr){
			auto value = target_type();
			for (; 0u < count; --count){
				switch (extract_value<machine::op_offset_operator>(memory, reg_table, exception)){
				case machine::op_offset_operator::plus:
					value += convert_operand_to_value<target_type>(extract_operand(sizeof(target_type), memory, reg_table, false));
					break;
				case machine::op_offset_operator::minus:
					value -= convert_operand_to_value<target_type>(extract_operand(sizeof(target_type), memory, reg_table, false));
					break;
				default://Error
					if (exception != nullptr)
						*exception = execution_code_access_protected;
					return target_type();
				}

				if (exception != nullptr && *exception != execution_code_success)
					return target_type();//Error
			}

			return value;
		}

		template <typename target_type>
		static target_type convert_operand_to_value(const operand_type &op, machine::memory *memory = nullptr){
			if (std::holds_alternative<machine::register_object *>(op))
				return std::get<machine::register_object *>(op)->read_scalar<target_type>();

			if (std::holds_alternative<byte *>(op))
				return convert_data_to_value<target_type>(std::get<byte *>(op));
			
			if (memory != nullptr && std::holds_alternative<qword>(op))
				return memory->read_scalar<target_type>(std::get<qword>(op));

			return target_type();
		}

		template <typename target_type>
		static target_type convert_data_to_value(byte *data){
			auto value = target_type();
			memcpy(&value, data, sizeof(target_type));
			return value;
		}

		static const unsigned int execution_code_success					= (0u << 0x0000u);
		static const unsigned int execution_code_unrecognized_instruction	= (1u << 0x0000u);
		static const unsigned int execution_code_access_protected			= (1u << 0x0001u);
		static const unsigned int execution_code_write_protected			= (1u << 0x0002u);

	protected:
		thread_local static byte temp_buffer_[sizeof(qword)];
		thread_local static byte temp_buffer2_[sizeof(qword)];
	};

	class mov_instruction : public instruction{
	public:
		virtual unsigned int execute(machine::memory &memory, machine::register_table &reg_table, machine::stack &stack) const override;
	};
}
