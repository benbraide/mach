#include "byte_code_instruction.h"

void mach::byte_code::instruction::execute(machine::memory &memory, machine::register_table &reg_table, machine::stack &stack){
	throw instruction_error_code::unrecognized_instruction;
}

mach::machine::register_object *mach::byte_code::instruction::get_register(std::size_t size, std::size_t index, machine::register_table &reg_table){
	std::string key;
	if (index < 16u)//Integer
		key = ("r" + std::to_string(index));
	else if (index < 24u)//Float
		key = ("xmm" + std::to_string(index - 16u));
	else//Error
		return nullptr;

	switch (size){
	case 1u:
		return reg_table.find(key + "b");
	case 2u:
		return reg_table.find(key + "w");
	case 4u:
		return reg_table.find(key + "d");
	case 8u:
		return reg_table.find(key);
	default:
		break;
	}

	return nullptr;
}

mach::byte_code::instruction::operand_type mach::byte_code::instruction::extract_operand(std::size_t size, machine::memory &memory, machine::register_table &reg_table, bool write_to_first_temp_buffer){
	switch (extract_value<machine::op_operand_type>(memory, reg_table)){
	case machine::op_operand_type::reg:
	{//Extract register
		auto data = extract_data(sizeof(byte), memory, reg_table, write_to_first_temp_buffer, instruction_error_code::bad_operand);
		if (data == nullptr)//Error
			return nullptr;

		return get_register(size, convert_data_to_value<byte>(data), reg_table);
	}
	case machine::op_operand_type::mem:
	{//Extract memory
		auto key_size = extract_value<byte>(memory, reg_table, instruction_error_code::bad_operand);
		if (key_size == 0u)//Error
			return nullptr;

		auto key_count = extract_value<byte>(memory, reg_table, instruction_error_code::bad_operand);
		if (key_count == 0u)//Error
			return nullptr;

		if (key_count == 1u){//Single key
			auto op = extract_operand(key_size, memory, reg_table, write_to_first_temp_buffer);
			if (!std::holds_alternative<byte *>(op))//Error
				throw instruction_error_code::bad_operand;

			switch (key_size){
			case 1u:
				return static_cast<qword>(convert_data_to_value<unsigned __int8>(std::get<byte *>(op)));
			case 2u:
				return static_cast<qword>(convert_data_to_value<unsigned __int16>(std::get<byte *>(op)));
			case 4u:
				return static_cast<qword>(convert_data_to_value<unsigned __int32>(std::get<byte *>(op)));
			case 8u:
				return convert_data_to_value<unsigned __int64>(std::get<byte *>(op));
			default://Error
				break;
			}

			return nullptr;
		}

		switch (key_size){
		case 1u:
			return static_cast<qword>(extract_offset<unsigned __int8>(key_count, memory, reg_table));
		case 2u:
			return static_cast<qword>(extract_offset<unsigned __int16>(key_count, memory, reg_table));
		case 4u:
			return static_cast<qword>(extract_offset<unsigned __int32>(key_count, memory, reg_table));
		case 8u:
			return extract_offset<unsigned __int64>(key_count, memory, reg_table);
		default://Error
			break;
		}

		return nullptr;
	}
	case machine::op_operand_type::val://Extract immediate value
		return extract_data(size, memory, reg_table, write_to_first_temp_buffer, instruction_error_code::bad_operand);
	default://Error
		return nullptr;
	}
}

mach::byte_code::instruction::byte *mach::byte_code::instruction::extract_data(std::size_t size, machine::memory &memory, machine::register_table &reg_table, bool write_to_first_temp_buffer, instruction_error_code e){
	auto ip = reg_table.get_instruction_pointer()->read_scalar<qword>();

	memory.read(ip, (write_to_first_temp_buffer ? temp_buffer : temp_buffer2), size);
	reg_table.get_instruction_pointer()->write_scalar(ip + size);

	return (write_to_first_temp_buffer ? temp_buffer : temp_buffer2);
}

thread_local mach::byte_code::instruction::byte mach::byte_code::instruction::temp_buffer[sizeof(qword)];

thread_local mach::byte_code::instruction::byte mach::byte_code::instruction::temp_buffer2[sizeof(qword)];

void mach::byte_code::nop_instruction::execute(machine::memory &memory, machine::register_table &reg_table, machine::stack &stack){}

void mach::byte_code::mov_instruction::execute(machine::memory &memory, machine::register_table &reg_table, machine::stack &stack){
	auto size = instruction::extract_value<instruction::byte>(memory, reg_table, instruction_error_code::bad_operand);

	auto destination = instruction::extract_operand(size, memory, reg_table, true);
	auto source = instruction::extract_operand(size, memory, reg_table, false);

	if (std::holds_alternative<instruction::qword>(destination)){//Memory destination
		if (std::holds_alternative<instruction::byte *>(source))//Value source
			memory.write(std::get<instruction::qword>(destination), std::get<instruction::byte *>(source), size);
		else if (std::holds_alternative<machine::register_object *>(source))//Register source
			memory.write(std::get<instruction::qword>(destination), std::get<machine::register_object *>(source)->get_data(), size);
		else if (std::holds_alternative<instruction::qword>(source))//Memory source
			memory.copy(std::get<instruction::qword>(source), std::get<instruction::qword>(destination), size);
	}
	else if (std::holds_alternative<machine::register_object *>(destination)){//Register destination
		if (std::holds_alternative<instruction::byte *>(source))//Value source
			std::get<machine::register_object *>(destination)->write(std::get<instruction::byte *>(source), size);
		else if (std::holds_alternative<machine::register_object *>(source))//Register source
			std::get<machine::register_object *>(destination)->write(std::get<machine::register_object *>(source)->get_data(), size);
		else if (std::holds_alternative<instruction::qword>(source))//Memory source
			memory.read(std::get<instruction::qword>(source), std::get<machine::register_object *>(destination)->get_data(), size);
	}
	else//Error
		throw instruction_error_code::bad_operand;
}
