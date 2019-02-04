#include "asm_code_translation_state.h"

mach::asm_code::translation_label::translation_label(const std::string &name, unsigned __int64 offset)
	: name_(name), offset_(offset){}

const std::string &mach::asm_code::translation_label::get_name() const{
	return name_;
}

unsigned __int64 mach::asm_code::translation_label::get_offset() const{
	return offset_;
}

mach::asm_code::translation_equ_label::translation_equ_label(const std::string &name, unsigned __int64 offset, std::shared_ptr<instruction_operand> expression)
	: translation_label(name, offset), expression_(expression){}

unsigned __int64 mach::asm_code::translation_equ_label::get_offset() const{
	return dynamic_cast<immediate_instruction_operand *>(expression_.get())->get_data_as<unsigned __int64>();
}

mach::asm_code::translation_section::translation_section(id_type id, translation_state &state)
	: id_(id), state_(&state){}

mach::asm_code::translation_label *mach::asm_code::translation_section::add_label(const std::string &name){
	if (state_->find_label(name) != nullptr)
		throw translation_error_code::duplicate_label;

	auto label = std::make_shared<translation_label>(name, offset_);
	entries_.push_back(label.get());

	return (labels_[name] = label).get();
}

void mach::asm_code::translation_section::add_equ(const std::string &name, std::shared_ptr<instruction_operand> op){
	if (state_->find_label(name) != nullptr)
		throw translation_error_code::duplicate_label;

	op->traverse_operands([&](const instruction_operand &operand){
		if (auto placeholder = dynamic_cast<placeholder_instruction_operand *>(const_cast<instruction_operand *>(&operand)); placeholder != nullptr){
			placeholder->set_data(offset_);
			placeholders_.push_back(placeholder);
		}
		else if (auto label_ref = dynamic_cast<label_ref_instruction_operand *>(const_cast<instruction_operand *>(&operand)); label_ref != nullptr)
			equ_label_refs_[label_ref->get_name()].push_back(label_ref);
	}, true);
}

void mach::asm_code::translation_section::add_instruction(std::shared_ptr<instruction> value){
	instructions_.push_back(value);
	entries_.push_back(value.get());

	auto meta_size = value->get_meta_encoded_size();
	value->traverse_operands([&](const instruction_operand &operand){
		if (auto placeholder = dynamic_cast<placeholder_instruction_operand *>(const_cast<instruction_operand *>(&operand)); placeholder != nullptr){
			placeholder->set_data(offset_ + meta_size);
			placeholders_.push_back(placeholder);
		}
		else if (auto label_ref = dynamic_cast<label_ref_instruction_operand *>(const_cast<instruction_operand *>(&operand)); label_ref != nullptr)
			label_refs_[label_ref->get_name()].push_back(label_ref);

		meta_size += operand.get_encoded_size(value->get_size_type());
	}, true);

	offset_ += meta_size;
}

mach::asm_code::translation_section::id_type mach::asm_code::translation_section::get_id() const{
	return id_;
}

mach::asm_code::translation_state *mach::asm_code::translation_section::get_state() const{
	return state_;
}

void mach::asm_code::translation_section::resolve(unsigned __int64 start_address){
	for (auto placeholder : placeholders_)//Update placeholders
		placeholder->set_data(placeholder->get_data_as<unsigned __int64>() + start_address);

	for (auto &label : labels_)//Update labels
		label.second->offset_ += start_address;

	for (auto &label_ref : equ_label_refs_){//Update label references
		auto label = find_label(label_ref.first);
		if (label == nullptr)
			throw translation_error_code::label_not_found;

		for (auto item : label_ref.second)
			item->set_data(label->get_offset());
	}

	for (auto &label : labels_){//Resolve EQU labels
		if (auto equ_label = dynamic_cast<translation_equ_label *>(label.second.get()); equ_label != nullptr)
			equ_label->expression_->resolve_operands();
	}

	for (auto &label_ref : label_refs_){//Update label references
		auto label = find_label(label_ref.first);
		if (label == nullptr)
			throw translation_error_code::label_not_found;

		for (auto item : label_ref.second)
			item->set_data(label->get_offset());
	}

	for (auto instruction : instructions_)
		instruction->resolve_operands();
}

unsigned __int64 mach::asm_code::translation_section::get_offset() const{
	return offset_;
}

mach::asm_code::translation_label *mach::asm_code::translation_section::find_label(const std::string &name) const{
	auto iter = labels_.find(name);
	return ((iter == labels_.end()) ? nullptr : iter->second.get());
}

mach::machine::register_table &mach::asm_code::translation_state::get_reg_table(){
	return reg_table_;
}

std::size_t mach::asm_code::translation_state::resolve(unsigned __int64 start_offset){
	auto offset = start_offset;
	for (auto &section : sections_){
		if (section.first != section_type::header){
			section.second->resolve(offset);
			offset += section.second->get_offset();
		}
	}

	return (offset - start_offset);
}

mach::asm_code::translation_label *mach::asm_code::translation_state::add_label(const std::string &name){
	if (current_section_ == nullptr)
		throw translation_error_code::no_active_section;

	return current_section_->add_label(name);
}

void mach::asm_code::translation_state::add_equ(const std::string &name, std::shared_ptr<instruction_operand> op){
	if (current_section_ != nullptr)
		current_section_->add_equ(name, op);
	else
		throw translation_error_code::no_active_section;
}

void mach::asm_code::translation_state::add_instruction(std::shared_ptr<instruction> value){
	if (current_section_ == nullptr)
		throw translation_error_code::no_active_section;

	value->validate_operands();
	current_section_->add_instruction(value);
}

mach::asm_code::translation_section *mach::asm_code::translation_state::set_current_section(section_type id){
	if (auto it = sections_.find(id); it == sections_.end())
		current_section_ = (sections_[id] = std::make_shared<translation_section>(id, *this)).get();
	else//Already exists
		current_section_ = it->second.get();

	return current_section_;
}

mach::asm_code::translation_section *mach::asm_code::translation_state::get_current_section() const{
	return current_section_;
}

mach::asm_code::translation_label *mach::asm_code::translation_state::find_label(const std::string &name) const{
	for (auto &section : sections_){
		if (auto found = section.second->find_label(name); found != nullptr)
			return found;
	}

	return nullptr;
}

void mach::asm_code::translation_state::enter_data_instruction(){
	is_inside_data_instruction_ = true;
}

void mach::asm_code::translation_state::leave_data_instruction(){
	is_inside_data_instruction_ = false;
}

bool mach::asm_code::translation_state::is_inside_data_instruction() const{
	return is_inside_data_instruction_;
}

void mach::asm_code::translation_state::set_stack_size(std::size_t value){
	stack_size_ = value;
}

std::size_t mach::asm_code::translation_state::get_stack_size() const{
	return stack_size_;
}

void mach::asm_code::translation_state::set_entry(const std::string &value){
	entry_ = value;
}

const std::string &mach::asm_code::translation_state::get_entry() const{
	return entry_;
}

unsigned __int64 mach::asm_code::translation_state::get_entry_address() const{
	auto label = find_label(entry_);
	return ((label == nullptr) ? 0u : label->get_offset());
}
