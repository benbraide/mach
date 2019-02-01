#include "asm_code_instruction.h"

mach::asm_code::instruction *mach::asm_code::instruction_operand::get_parent() const{
	return parent_;
}

mach::machine::op_operand_size mach::asm_code::instruction_operand::get_size_type() const{
	return machine::op_operand_size::nil;
}

void mach::asm_code::instruction_operand::validate_operands(machine::op_operand_size size_type) const{
	if ((size_type != machine::op_operand_size::nil || get_size_type() != machine::op_operand_size::nil) && get_size_type() != size_type)
		throw instruction_error_code::mismatched_operand_types;
}

void mach::asm_code::instruction_operand::resolve_operands(){}

void mach::asm_code::instruction_operand::traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep) const{
	callback(*this);
}

void mach::asm_code::instruction_operand::set_parent_(instruction &parent){
	parent_ = &parent;
}

void mach::asm_code::instruction_operand::set_parent_of_(instruction_operand &op, instruction &parent){
	op.set_parent_(parent);
}

bool mach::asm_code::instruction_operand::is_data_operand_() const{
	return (parent_ != nullptr && parent_->is_data_instruction());
}

mach::asm_code::register_instruction_operand::register_instruction_operand(const std::string &name, machine::register_object &reg)
	: name_(name), reg_(&reg){}

mach::asm_code::register_instruction_operand::register_instruction_operand(std::string &&name, machine::register_object &reg)
	: name_(std::move(name)), reg_(&reg){}

void mach::asm_code::register_instruction_operand::print(io::stream_writer &writer) const{
	writer.write(name_.data(), name_.size());
}

void mach::asm_code::register_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	writer.write_scalar(machine::op_operand_type::reg);
	writer.write_scalar(reg_->get_index());
}

mach::machine::op_operand_type mach::asm_code::register_instruction_operand::get_type() const{
	return machine::op_operand_type::reg;
}

mach::machine::op_operand_size mach::asm_code::register_instruction_operand::get_size_type() const{
	return reg_->get_size_type();
}

std::size_t mach::asm_code::register_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return (sizeof(machine::op_operand_type) + sizeof(byte));
}

mach::asm_code::memory_instruction_operand::memory_instruction_operand(std::shared_ptr<instruction_operand> key, machine::op_operand_size size_type)
	: key_(key), size_type_(size_type){}

void mach::asm_code::memory_instruction_operand::print(io::stream_writer &writer) const{
	writer.write('[');
	key_->print(writer);
	writer.write(']');
}

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

void mach::asm_code::memory_instruction_operand::validate_operands(machine::op_operand_size size_type) const{
	instruction_operand::validate_operands(size_type);
	key_->validate_operands(size_type);
}

void mach::asm_code::memory_instruction_operand::resolve_operands(){
	key_->resolve_operands();
}

void mach::asm_code::memory_instruction_operand::set_parent_(instruction &parent){
	instruction_operand::set_parent_(parent);
	instruction_operand::set_parent_of_(*key_, parent);
}

mach::asm_code::string_instruction_operand::string_instruction_operand(const std::string &data)
	: data_(data){}

mach::asm_code::string_instruction_operand::string_instruction_operand(std::string &&data)
	: data_(std::move(data)){}

void mach::asm_code::string_instruction_operand::print(io::stream_writer &writer) const{
	std::string escaped_data;
	escaped_data.reserve(data_.size() + 4u);

	for (auto c : data_){//Check for escapes
		switch (c){
		case '\0':
			escaped_data += "\\0";
			break;
		case '\a':
			escaped_data += "\\a";
			break;
		case '\b':
			escaped_data += "\\b";
			break;
		case '\f':
			escaped_data += "\\f";
			break;
		case '\n':
			escaped_data += "\\n";
			break;
		case '\r':
			escaped_data += "\\r";
			break;
		case '\t':
			escaped_data += "\\t";
			break;
		case '\v':
			escaped_data += "\\v";
			break;
		default:
			escaped_data.push_back(c);
			break;
		}
	}

	writer.write('\'');
	writer.write(escaped_data.data(), escaped_data.size());
	writer.write('\'');
}

void mach::asm_code::string_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	writer.write_buffer(data_.data(), data_.size());
}

mach::machine::op_operand_type mach::asm_code::string_instruction_operand::get_type() const{
	return machine::op_operand_type::val;
}

mach::machine::op_operand_size mach::asm_code::string_instruction_operand::get_size_type() const{
	return machine::op_operand_size::byte;
}

std::size_t mach::asm_code::string_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return data_.size();
}

const std::string &mach::asm_code::string_instruction_operand::get_data() const{
	return data_;
}

mach::asm_code::immediate_instruction_operand::immediate_instruction_operand(){
	memset(data_, 0, sizeof(unsigned __int64));
}

mach::asm_code::immediate_instruction_operand::immediate_instruction_operand(const byte *data, std::size_t size, held_value_type type)
	: type_(type){
	memcpy(data_, data, ((size < sizeof(unsigned __int64)) ? size : sizeof(unsigned __int64)));
	if (size < sizeof(unsigned __int64))//Zero higher bits
		memset((data_ + size), 0, (sizeof(unsigned __int64) - size));
}

void mach::asm_code::immediate_instruction_operand::print(io::stream_writer &writer) const{
	switch (type_){
	case held_value_type::char_:
		writer.write(get_data_as<char>());
		break;
	case held_value_type::float_:
		writer.write(get_data_as<long double>());
		break;
	default:
		writer.write(get_data_as<unsigned __int64>());
		break;
	}
}

void mach::asm_code::immediate_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	if (!is_data_operand_())
		writer.write_scalar(machine::op_operand_type::val);
	writer.write(data_, ((static_cast<std::size_t>(size_type) < sizeof(unsigned __int64)) ? static_cast<std::size_t>(size_type) : sizeof(unsigned __int64)));
}

mach::machine::op_operand_type mach::asm_code::immediate_instruction_operand::get_type() const{
	return machine::op_operand_type::val;
}

mach::machine::op_operand_size mach::asm_code::immediate_instruction_operand::get_size_type() const{
	return ((type_ == held_value_type::char_) ? machine::op_operand_size::byte : machine::op_operand_size::nil);
}

std::size_t mach::asm_code::immediate_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return (is_data_operand_() ? static_cast<std::size_t>(size_type) : (sizeof(machine::op_operand_type) + static_cast<std::size_t>(size_type)));
}

bool mach::asm_code::immediate_instruction_operand::is_float_value() const{
	return (type_ == held_value_type::float_);
}

mach::asm_code::immediate_instruction_operand::held_value_type mach::asm_code::immediate_instruction_operand::get_held_type() const{
	return type_;
}

mach::asm_code::instruction_operand::byte *mach::asm_code::immediate_instruction_operand::get_data() const{
	return data_;
}

mach::asm_code::expression_instruction_operand::expression_instruction_operand(char op, std::shared_ptr<instruction_operand> left, std::shared_ptr<instruction_operand> right)
	: op_(op), left_(left), right_(right){}

void mach::asm_code::expression_instruction_operand::print(io::stream_writer &writer) const{
	left_->print(writer);
	writer.write(op_);
	right_->print(writer);
}

void mach::asm_code::expression_instruction_operand::validate_operands(machine::op_operand_size size_type) const{
	instruction_operand::validate_operands(size_type);

	left_->validate_operands(size_type);
	right_->validate_operands(size_type);

	auto im_left = dynamic_cast<immediate_instruction_operand *>(left_.get()), im_right = dynamic_cast<immediate_instruction_operand *>(right_.get());
	if (im_left == nullptr || im_right == nullptr)
		throw instruction_error_code::bad_operand;

	if (im_left->get_held_type() != im_right->get_held_type() && im_left->is_float_value() != im_right->is_float_value())
		throw instruction_error_code::mismatched_operand_types;
}

void mach::asm_code::expression_instruction_operand::resolve_operands(){
	left_->resolve_operands();
	right_->resolve_operands();
	
	auto im_left = dynamic_cast<immediate_instruction_operand *>(left_.get()), im_right = dynamic_cast<immediate_instruction_operand *>(right_.get());
	switch (op_){
	case '+':
		if (im_left->is_float_value() || im_right->is_float_value())
			set_data(im_left->get_data_as<long double>() + im_right->get_data_as<long double>());
		else//Integral values
			set_data(im_left->get_data_as<unsigned __int64>() + im_right->get_data_as<unsigned __int64>());
		break;
	case '-':
		if (im_left->is_float_value() || im_right->is_float_value())
			set_data(im_left->get_data_as<long double>() - im_right->get_data_as<long double>());
		else//Integral values
			set_data(im_left->get_data_as<unsigned __int64>() - im_right->get_data_as<unsigned __int64>());
		break;
	case '*':
		if (im_left->is_float_value() || im_right->is_float_value())
			set_data(im_left->get_data_as<long double>() * im_right->get_data_as<long double>());
		else//Integral values
			set_data(im_left->get_data_as<unsigned __int64>() * im_right->get_data_as<unsigned __int64>());
		break;
	case '/':
		if (im_left->is_float_value() || im_right->is_float_value())
			set_data(im_left->get_data_as<long double>() / im_right->get_data_as<long double>());
		else//Integral values
			set_data(im_left->get_data_as<unsigned __int64>() / im_right->get_data_as<unsigned __int64>());
		break;
	default:
		break;
	}
}

void mach::asm_code::expression_instruction_operand::traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep) const{
	callback(*this);
	left_->traverse_operands(callback, deep);
	right_->traverse_operands(callback, deep);
}

void mach::asm_code::expression_instruction_operand::set_parent_(instruction &parent){
	instruction_operand::set_parent_(parent);
	instruction_operand::set_parent_of_(*left_, parent);
	instruction_operand::set_parent_of_(*right_, parent);
}

mach::asm_code::label_ref_instruction_operand::label_ref_instruction_operand(const std::string &name)
	: name_(name){}

void mach::asm_code::label_ref_instruction_operand::print(io::stream_writer &writer) const{
	writer.write(name_.data(), name_.size());
}

const std::string &mach::asm_code::label_ref_instruction_operand::get_name() const{
	return name_;
}

mach::asm_code::placeholder_instruction_operand::placeholder_instruction_operand()
	: label_ref_instruction_operand(""){}

void mach::asm_code::placeholder_instruction_operand::print(io::stream_writer &writer) const{
	writer.write('$');
}

void mach::asm_code::uninitialized_instruction_operand::print(io::stream_writer &writer) const{
	writer.write('?');
}

void mach::asm_code::uninitialized_instruction_operand::encode(machine::op_operand_size size_type, io::writer &writer, machine::register_table &reg_table) const{
	writer.advance(static_cast<std::size_t>(size_type));
}

mach::machine::op_operand_type mach::asm_code::uninitialized_instruction_operand::get_type() const{
	return machine::op_operand_type::val;
}

std::size_t mach::asm_code::uninitialized_instruction_operand::get_encoded_size(machine::op_operand_size size_type) const{
	return static_cast<std::size_t>(size_type);
}

mach::asm_code::offset_instruction_operand::offset_instruction_operand(const std::vector<part_info> &parts, bool is_explicit)
	: parts_(parts), is_explicit_(is_explicit){
	resolve_size_type_();
}

mach::asm_code::offset_instruction_operand::offset_instruction_operand(std::vector<part_info> &&parts, bool is_explicit)
	: parts_(std::move(parts)), is_explicit_(is_explicit){
	resolve_size_type_();
}

void mach::asm_code::offset_instruction_operand::print(io::stream_writer &writer) const{
	if (is_explicit_)
		writer.write("offset ", 7u);

	auto count = 0;
	for (auto &part : parts_){
		if (count == 0)
			++count;
		else
			writer.write((part.op == machine::op_offset_operator::minus) ? '-' : '+');
		part.operand->print(writer);
	}
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

void mach::asm_code::offset_instruction_operand::traverse_operands(const std::function<void(const instruction_operand &)> &callback, bool deep) const{
	callback(*this);
	for (auto &part : parts_){
		if (deep)
			part.operand->traverse_operands(callback, true);
		else
			callback(*part.operand);
	}
}

void mach::asm_code::offset_instruction_operand::set_parent_(instruction &parent){
	instruction_operand::set_parent_(parent);
	for (auto &part : parts_)
		instruction_operand::set_parent_of_(*part.operand, parent);
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
