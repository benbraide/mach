#include "asm_code_instruction.h"

mach::asm_code::instruction::instruction(const operand_type &operands)
	: operands_(operands){
	resolve_size_type_();
}

mach::asm_code::instruction::instruction(operand_type &&operands)
	: operands_(std::move(operands)){
	resolve_size_type_();
}

void mach::asm_code::instruction::print(io::stream_writer &writer) const{
	print_meta(writer);
	print_operands(writer);
}

void mach::asm_code::instruction::print_meta(io::stream_writer &writer) const{
	if (auto op_string = get_op_code_string(get_op_code()); !op_string.empty())
		writer.write(op_string.data(), op_string.size());
}

void mach::asm_code::instruction::print_operands(io::stream_writer &writer) const{
	std::size_t count = 0u;
	for (auto operand : operands_){
		writer.write(' ');
		operand->print(writer);
	}
}

void mach::asm_code::instruction::encode(io::writer &writer, machine::register_table &reg_table) const{
	if (size_type_ != machine::op_operand_size::error){
		encode_meta(writer);
		encode_operands(writer, reg_table);
	}
}

void mach::asm_code::instruction::encode_meta(io::writer &writer) const{
	if (is_data_instruction())
		return;//No meta

	writer.write_scalar(get_op_code());
	if (!operands_.empty())
		writer.write_scalar(get_size_type());
}

void mach::asm_code::instruction::encode_operands(io::writer &writer, machine::register_table &reg_table) const{
	for (auto operand : operands_)
		operand->encode(size_type_, writer, reg_table);
}

mach::machine::op_operand_size mach::asm_code::instruction::get_size_type() const{
	return size_type_;
}

std::size_t mach::asm_code::instruction::get_encoded_size() const{
	return ((size_type_ == machine::op_operand_size::error) ? 0u : (get_meta_encoded_size() + get_operands_encoded_size()));
}

std::size_t mach::asm_code::instruction::get_meta_encoded_size() const{
	if (is_data_instruction())
		return 0u;//No meta
	return (operands_.empty() ? sizeof(machine::op_code) : (sizeof(machine::op_code) + sizeof(machine::op_operand_size)));
}

std::size_t mach::asm_code::instruction::get_operands_encoded_size() const{
	std::size_t operands_encoded_size = 0u;
	for (auto operand : operands_)
		operands_encoded_size += operand->get_encoded_size(size_type_);

	return operands_encoded_size;
}

void mach::asm_code::instruction::validate_operands() const{
	auto required_operand_count = get_required_operand_count_();
	if (operands_.size() < required_operand_count.first || required_operand_count.second < operands_.size())
		throw instruction_error_code::bad_operand_count;

	if (size_type_ == machine::op_operand_size::error)
		throw instruction_error_code::mismatched_operand_types;

	if (size_type_ == machine::op_operand_size::nil && !operands_.empty())
		throw instruction_error_code::ambiguous_instruction;

	std::size_t index = 0u;
	for (auto operand : operands_){//Validate operands size types
		operand->validate_operands(get_size_type());
		if (!is_valid_operand_type_(operand->get_type(), index++) || !is_valid_size_type_(operand->get_size_type()))
			throw instruction_error_code::bad_operand;
	}
}

void mach::asm_code::instruction::resolve_operands(){
	for (auto operand : operands_)
		operand->resolve_operands();
}

void mach::asm_code::instruction::traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep) const{
	for (auto operand : operands_){
		if (deep)
			operand->traverse_operands(callback, true);
		else
			callback(*operand);
	}
}

bool mach::asm_code::instruction::is_data_instruction() const{
	return false;
}

std::string mach::asm_code::instruction::get_op_code_string(machine::op_code op_code){
	switch (op_code){
	case machine::op_code::nop:
		return "nop";
	case machine::op_code::push:
		return "push";
	case machine::op_code::pop:
		return "pop";
	case machine::op_code::mov:
		return "mov";
	case machine::op_code::lea:
		return "lea";
	case machine::op_code::syscall:
		return "syscall";
	case machine::op_code::call:
		return "call";
	case machine::op_code::enter:
		return "enter";
	case machine::op_code::leave:
		return "leave";
	case machine::op_code::ret:
		return "ret";
	case machine::op_code::jmp:
		return "jmp";
	case machine::op_code::jz:
		return "jz";
	case machine::op_code::jnz:
		return "jnz";
	case machine::op_code::je:
		return "je";
	case machine::op_code::jne:
		return "jne";
	case machine::op_code::jl:
		return "jl";
	case machine::op_code::jnl:
		return "jnl";
	case machine::op_code::jle:
		return "jle";
	case machine::op_code::jnle:
		return "jnle";
	case machine::op_code::jg:
		return "jg";
	case machine::op_code::jng:
		return "jng";
	case machine::op_code::jge:
		return "jge";
	case machine::op_code::jnge:
		return "jnge";
	case machine::op_code::setz:
		return "setz";
	case machine::op_code::setnz:
		return "setnz";
	case machine::op_code::sete:
		return "sete";
	case machine::op_code::setne:
		return "setne";
	case machine::op_code::setl:
		return "setl";
	case machine::op_code::setnl:
		return "setnl";
	case machine::op_code::setle:
		return "setle";
	case machine::op_code::setnle:
		return "setnle";
	case machine::op_code::setg:
		return "setg";
	case machine::op_code::setng:
		return "setng";
	case machine::op_code::setge:
		return "setge";
	case machine::op_code::setnge:
		return "setnge";
	case machine::op_code::loop:
		return "loop";
	case machine::op_code::inc:
		return "inc";
	case machine::op_code::dec:
		return "dec";
	case machine::op_code::add:
		return "add";
	case machine::op_code::sub:
		return "sub";
	case machine::op_code::mult:
		return "mult";
	case machine::op_code::div:
		return "div";
	case machine::op_code::mod:
		return "mod";
	case machine::op_code::and_:
		return "and";
	case machine::op_code::xor_:
		return "xor";
	case machine::op_code::or_:
		return "or";
	case machine::op_code::sal:
		return "sal";
	case machine::op_code::sar:
		return "sar";
	case machine::op_code::test:
		return "test";
	case machine::op_code::not_:
		return "not";
	case machine::op_code::cmp:
		return "cmp";
	case machine::op_code::cnvt:
		return "cnvt";
	default:
		break;
	}

	return "";
}

void mach::asm_code::instruction::resolve_size_type_(){
	for (auto operand : operands_){//Get common size type
		if ((size_type_ = operand->get_size_type()) != machine::op_operand_size::nil)
			break;
	}
}

mach::asm_code::instruction::size_pair_type mach::asm_code::instruction::get_required_operand_count_() const{
	return size_pair_type{};
}

bool mach::asm_code::instruction::is_valid_size_type_(machine::op_operand_size size_type) const{
	return true;
}

bool mach::asm_code::instruction::is_valid_operand_type_(machine::op_operand_type operand_type, std::size_t index) const{
	return true;
}

mach::asm_code::data_instruction::data_instruction(const operand_type &operands)
	: instruction(operands){}

mach::asm_code::data_instruction::data_instruction(operand_type &&operands)
	: instruction(std::move(operands)){}

mach::machine::op_code mach::asm_code::data_instruction::get_op_code() const{
	return machine::op_code::nop;
}

bool mach::asm_code::data_instruction::is_data_instruction() const{
	return true;
}

mach::asm_code::zero_instruction::zero_instruction(unsigned __int64 size)
	: data_instruction({}), size_(size){}

void mach::asm_code::zero_instruction::print_meta(io::stream_writer &writer) const{
	writer.write(".zero", 5u);
}

void mach::asm_code::zero_instruction::encode_operands(io::writer &writer, machine::register_table &reg_table) const{
	writer.set(static_cast<byte>(0), size_);
}

mach::machine::op_operand_size mach::asm_code::zero_instruction::get_size_type() const{
	return machine::op_operand_size::byte;
}

std::size_t mach::asm_code::zero_instruction::get_operands_encoded_size() const{
	return size_;
}

mach::asm_code::string_instruction::string_instruction(const std::string &data)
	: data_instruction({}), data_(data){}

mach::asm_code::string_instruction::string_instruction(std::string &&data)
	: data_instruction({}), data_(std::move(data)){}

void mach::asm_code::string_instruction::print_meta(io::stream_writer &writer) const{
	writer.write(".string", 7u);
}

void mach::asm_code::string_instruction::encode_operands(io::writer &writer, machine::register_table &reg_table) const{
	writer.write_buffer(data_.data(), data_.size());
}

mach::machine::op_operand_size mach::asm_code::string_instruction::get_size_type() const{
	return machine::op_operand_size::byte;
}

std::size_t mach::asm_code::string_instruction::get_operands_encoded_size() const{
	return data_.size();
}
