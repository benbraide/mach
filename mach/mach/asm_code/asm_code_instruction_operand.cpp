#include "asm_code_instruction_operand.h"

mach::machine::op_operand_size mach::asm_code::instruction_operand::get_size_type() const{
	return machine::op_operand_size::nil;
}

mach::asm_code::regiester_instruction_operand::regiester_instruction_operand(machine::register_object &reg)
	: reg_(&reg){}

void mach::asm_code::regiester_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	writer.write_scalar(machine::op_operand_type::reg);
	writer.write_scalar(reg_->get_index());
}

mach::machine::op_operand_type mach::asm_code::regiester_instruction_operand::get_type() const{
	return machine::op_operand_type::reg;
}

mach::machine::op_operand_size mach::asm_code::regiester_instruction_operand::get_size_type() const{
	return reg_->get_size_type();
}

std::size_t mach::asm_code::regiester_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return (sizeof(machine::op_operand_type) + sizeof(byte));
}

mach::asm_code::memory_instruction_operand::memory_instruction_operand(std::shared_ptr<instruction_operand> key, machine::op_operand_size size_type)
	: key_(key), size_type_(size_type){}

void mach::asm_code::memory_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	if (dynamic_cast<offset_instruction_operand *>(key_.get()) == nullptr)//Single key
		writer.write_scalar(machine::op_operand_type::mem);
	else//Key is an offset
		writer.write_scalar(machine::op_operand_type::mem_with_offset);

	if (auto key_size_type = key_->get_size_type(); key_size_type != machine::op_operand_size::nil)
		writer.write_scalar(key_size_type);
	else//Key is not typed
		writer.write_scalar(machine::op_operand_size::qword);

	key_->encode(machine::op_operand_size::qword, writer, reg_table);
}

mach::machine::op_operand_type mach::asm_code::memory_instruction_operand::get_type() const{
	return ((dynamic_cast<offset_instruction_operand *>(key_.get()) == nullptr) ? machine::op_operand_type::mem : machine::op_operand_type::mem_with_offset);
}

mach::machine::op_operand_size mach::asm_code::memory_instruction_operand::get_size_type() const{
	return size_type_;
}

std::size_t mach::asm_code::memory_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return (sizeof(machine::op_operand_type) + sizeof(machine::op_operand_size) + key_->get_encoded_size(machine::op_operand_size::qword));
}

mach::asm_code::immediate_instruction_operand::immediate_instruction_operand(){
	memset(data_, 0, sizeof(unsigned __int64));
}

mach::asm_code::immediate_instruction_operand::immediate_instruction_operand(const byte *data, std::size_t size){
	memset(data_, 0, sizeof(unsigned __int64));
	memcpy(data_, data, ((size < sizeof(unsigned __int64)) ? size : sizeof(unsigned __int64)));
}

void mach::asm_code::immediate_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	writer.write_scalar(machine::op_operand_type::val);
	writer.write(data_, ((static_cast<std::size_t>(size_type) < sizeof(unsigned __int64)) ? static_cast<std::size_t>(size_type) : sizeof(unsigned __int64)));
}

mach::machine::op_operand_type mach::asm_code::immediate_instruction_operand::get_type() const{
	return machine::op_operand_type::val;
}

std::size_t mach::asm_code::immediate_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return (sizeof(machine::op_operand_type) + static_cast<std::size_t>(size_type));
}

mach::asm_code::instruction_operand::byte *mach::asm_code::immediate_instruction_operand::get_data() const{
	return data_;
}

mach::asm_code::label_ref_instruction_operand::label_ref_instruction_operand(const std::string &name)
	: name_(name){}

const std::string &mach::asm_code::label_ref_instruction_operand::get_name() const{
	return name_;
}

mach::asm_code::placeholder_instruction_operand::placeholder_instruction_operand()
	: label_ref_instruction_operand(""){}

void mach::asm_code::uninitialized_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	//Offset writer
}

mach::machine::op_operand_type mach::asm_code::uninitialized_instruction_operand::get_type() const{
	return machine::op_operand_type::val;
}

std::size_t mach::asm_code::uninitialized_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return static_cast<std::size_t>(size_type);
}

mach::asm_code::offset_instruction_operand::offset_instruction_operand(const std::vector<part_info> &parts)
	: parts_(parts){
	resolve_size_type_();
}

mach::asm_code::offset_instruction_operand::offset_instruction_operand(std::vector<part_info> &&parts)
	: parts_(std::move(parts)){
	resolve_size_type_();
}

void mach::asm_code::offset_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	writer.write_scalar(static_cast<byte>(parts_.size()));
	for (auto &part : parts_){//Write parts
		writer.write_scalar(part.op);
		part.operand->encode((size_type_ == machine::op_operand_size::nil) ? size_type : size_type_, writer, reg_table);
	}
}

mach::machine::op_operand_type mach::asm_code::offset_instruction_operand::get_type() const{
	return machine::op_operand_type::nil;
}

mach::machine::op_operand_size mach::asm_code::offset_instruction_operand::get_size_type() const{
	return size_type_;
}

std::size_t mach::asm_code::offset_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	if (size_type_ != machine::op_operand_size::nil)
		size_type = size_type_;//Use this size type

	return (sizeof(machine::op_operand_size) + sizeof(machine::op_operand_type) + sizeof(byte) + (sizeof(machine::op_offset_operator) * parts_.size()) + (static_cast<std::size_t>(size_type) * parts_.size()));
}

const std::vector<mach::asm_code::offset_instruction_operand::part_info> &mach::asm_code::offset_instruction_operand::get_parts() const{
	return parts_;
}

void mach::asm_code::offset_instruction_operand::resolve_size_type_(){
	for (auto &part : parts_){//Get common size type, if any
		if (dynamic_cast<immediate_instruction_operand *>(part.operand.get()) == nullptr){//Error
			size_type_ = machine::op_operand_size::error;
			break;
		}

		if (auto size_type = part.operand->get_size_type(); size_type != machine::op_operand_size::nil){
			if (size_type == machine::op_operand_size::dfloat || size_type == machine::op_operand_size::qfloat || (size_type_ != machine::op_operand_size::nil && size_type != size_type_)){//Error
				size_type_ = machine::op_operand_size::error;
				break;
			}

			size_type_ = size_type;
		}
	}
}
