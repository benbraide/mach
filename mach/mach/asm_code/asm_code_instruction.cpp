#include "asm_code_instruction.h"

mach::asm_code::instruction::instruction(const std::vector<std::shared_ptr<instruction_operand>> &operands)
	: operands_(operands){
	resolve_size_type_();
}

mach::asm_code::instruction::instruction(std::vector<std::shared_ptr<instruction_operand>> &&operands)
	: operands_(std::move(operands)){
	resolve_size_type_();
}

void mach::asm_code::instruction::encode(io::writer &writer, machine::register_table &reg_table) const{
	if (size_type_ == machine::op_operand_size::error)
		return;//Error

	writer.write_scalar(get_op_code());
	if (!operands_.empty())
		writer.write_scalar(get_size_type());

	for (auto operand : operands_)
		operand->encode(size_type_, writer, reg_table);
}

mach::machine::op_operand_size mach::asm_code::instruction::get_size_type() const{
	return size_type_;
}

std::size_t mach::asm_code::instruction::get_encoded_size() const{
	if (size_type_ == machine::op_operand_size::error)
		return 0u;//Error

	std::size_t operands_encoded_size = 0u;
	for (auto operand : operands_)
		operands_encoded_size += operand->get_encoded_size(size_type_);

	if (operands_.empty())
		return (sizeof(machine::op_code) + operands_encoded_size);

	return (sizeof(machine::op_code) + sizeof(machine::op_operand_size) + operands_encoded_size);
}

void mach::asm_code::instruction::validate_operands() const{
	if (operands_.size() != get_required_operand_count_())
		throw instruction_error_code::bad_operand_count;

	if (size_type_ == machine::op_operand_size::error)
		throw instruction_error_code::mismatched_operand_types;

	if (size_type_ == machine::op_operand_size::nil && !operands_.empty())
		throw instruction_error_code::ambiguous_instruction;

	std::size_t index = 0u;
	for (auto operand : operands_){//Validate operands size types
		if (!is_valid_operand_type_(operand->get_type(), index++) || !is_valid_size_type_(operand->get_size_type()))
			throw instruction_error_code::bad_operand;
	}
}

void mach::asm_code::instruction::resolve_size_type_(){
	for (auto operand : operands_){//Get common size type
		if (auto size_type = operand->get_size_type(); size_type != machine::op_operand_size::nil){
			if (size_type == machine::op_operand_size::error || (size_type_ != machine::op_operand_size::nil && size_type != size_type_)){//Error
				size_type_ = machine::op_operand_size::error;
				break;
			}

			size_type_ = size_type;
		}
	}
}

std::size_t mach::asm_code::instruction::get_required_operand_count_() const{
	return 0u;
}

bool mach::asm_code::instruction::is_valid_size_type_(machine::op_operand_size size_type) const{
	return true;
}

bool mach::asm_code::instruction::is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const{
	return true;
}

mach::asm_code::mov_instruction::mov_instruction(const std::vector<std::shared_ptr<instruction_operand>> &operands)
	: instruction(operands){}

mach::asm_code::mov_instruction::mov_instruction(std::vector<std::shared_ptr<instruction_operand>> &&operands)
	: instruction(std::move(operands)){}

mach::machine::op_code mach::asm_code::mov_instruction::get_op_code() const{
	return machine::op_code::mov;
}

std::size_t mach::asm_code::mov_instruction::get_required_operand_count_() const{
	return 2u;
}

bool mach::asm_code::mov_instruction::is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const{
	switch (operand_type){
	case machine::op_operand_type::reg:
	case machine::op_operand_type::mem:
	case machine::op_operand_type::mem_with_offset:
		return true;
	case machine::op_operand_type::val:
		return (index == 1u);
	default:
		break;
	}

	return false;
}
