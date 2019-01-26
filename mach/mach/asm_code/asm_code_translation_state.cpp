#include "asm_code_translation_state.h"

mach::asm_code::translation_label::translation_label(const std::string &name, translation_label *parent)
	: name_(name), parent_(parent), qname_((parent == nullptr) ? name : (parent->get_qname() + "." + name)){}

mach::asm_code::translation_label *mach::asm_code::translation_label::add_label(const std::string &name){
	auto label = std::make_shared<translation_label>(name, this);

	entries_.push_back(label.get());
	labels_[name] = label;

	return label.get();
}

void mach::asm_code::translation_label::add_instruction(std::shared_ptr<instruction> value){
	instructions_.push_back(value);
	entries_.push_back(value.get());
}

const std::string &mach::asm_code::translation_label::get_name() const{
	return name_;
}

const std::string &mach::asm_code::translation_label::get_qname() const{
	return name_;
}

mach::asm_code::translation_label *mach::asm_code::translation_label::get_parent() const{
	return parent_;
}

mach::asm_code::translation_label *mach::asm_code::translation_label::find_label(const std::string &name) const{
	auto iter = labels_.find(name);
	return ((iter == labels_.end()) ? nullptr : iter->second.get());
}

mach::asm_code::translation_label *mach::asm_code::translation_label::find_qlabel(const std::string &qname) const{
	for (auto &label : labels_){
		if (label.second->get_qname() == qname)
			return label.second.get();

		if (auto found = label.second->find_qlabel(qname); found != nullptr)
			return found;
	}

	return nullptr;
}

mach::asm_code::translation_section::translation_section(id_type id, translation_state &state)
	: id_(id), state_(&state){}

mach::asm_code::translation_label *mach::asm_code::translation_section::add_label(const std::string &name, bool nested){
	if (nested && current_label_ != nullptr)//Nest inside current label
		return add_label(name, current_label_);

	if (state_->find_qlabel(name) != nullptr)
		throw translation_error_code::duplicate_label;

	auto label = std::make_shared<translation_label>(name);
	entries_.push_back(label.get());

	current_label_ = (labels_[name] = label).get();
	label_offsets_[current_label_->get_qname()] = offset_;

	return current_label_;
}

mach::asm_code::translation_label *mach::asm_code::translation_section::add_label(const std::string &name, translation_label *parent){
	if (parent == nullptr)//No parent
		return add_label(name, false);

	if (state_->find_qlabel(parent->get_qname() + "." + name) != nullptr)
		throw translation_error_code::duplicate_label;

	current_label_ = current_label_->add_label(name);
	label_offsets_[current_label_->get_qname()] = offset_;

	return current_label_;
}

void mach::asm_code::translation_section::add_instruction(std::shared_ptr<instruction> value){
	if (current_label_ == nullptr){
		instructions_.push_back(value);
		entries_.push_back(value.get());
	}
	else//Use current label
		current_label_->add_instruction(value);

	auto meta_size = value->get_meta_encoded_size();
	value->traverse_operands([&](const instruction_operand &operand){
		if (auto placeholder = dynamic_cast<placeholder_instruction_operand *>(const_cast<instruction_operand *>(&operand)); placeholder != nullptr){
			placeholder->set_data(offset_ + meta_size);
			label_refs_[""].push_back(placeholder);
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

void mach::asm_code::translation_section::update_offset(unsigned __int64 value){
	for (auto &offset : label_offsets_){//Update label offsets and references
		offset.second += value;
		if (auto it = label_refs_.find(offset.first); it != label_refs_.end()){//Update references
			for (auto item : it->second)
				item->set_data(offset.second);
		}
	}

	if (auto it = label_refs_.find(""); it != label_refs_.end()){//Update placeholders
		for (auto item : it->second)
			item->set_data(item->get_data_as<unsigned __int64>() + value);
	}

	for (auto instruction : instructions_)
		instruction->resolve_operands();
}

unsigned __int64 mach::asm_code::translation_section::get_offset() const{
	return offset_;
}

mach::asm_code::translation_label *mach::asm_code::translation_section::get_current_label() const{
	return current_label_;
}

mach::asm_code::translation_label *mach::asm_code::translation_section::find_label(const std::string &name) const{
	auto iter = labels_.find(name);
	return ((iter == labels_.end()) ? nullptr : iter->second.get());
}

mach::asm_code::translation_label *mach::asm_code::translation_section::find_qlabel(const std::string &qname) const{
	for (auto &label : labels_){
		if (label.second->get_qname() == qname)
			return label.second.get();

		if (auto found = label.second->find_qlabel(qname); found != nullptr)
			return found;
	}

	return nullptr;
}

mach::machine::register_table &mach::asm_code::translation_state::get_reg_table(){
	return reg_table_;
}

void mach::asm_code::translation_state::update_offsets(unsigned __int64 start_offset){
	for (auto &section : sections_){
		if (section.first != section_type::header){
			section.second->update_offset(start_offset);
			start_offset += section.second->get_offset();
		}
	}
}

mach::asm_code::translation_label *mach::asm_code::translation_state::add_label(const std::string &name, bool nested){
	if (current_section_ == nullptr)
		throw translation_error_code::no_active_section;

	return current_section_->add_label(name, nested);
}

mach::asm_code::translation_label *mach::asm_code::translation_state::add_label(const std::string &name, translation_label *parent){
	if (current_section_ == nullptr)
		throw translation_error_code::no_active_section;

	return current_section_->add_label(name, parent);
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

mach::asm_code::translation_label *mach::asm_code::translation_state::get_current_label() const{
	return ((current_section_ == nullptr) ? nullptr : current_section_->get_current_label());
}

mach::asm_code::translation_label *mach::asm_code::translation_state::find_qlabel(const std::string &qname) const{
	for (auto &section : sections_){
		if (auto found = section.second->find_qlabel(qname); found != nullptr)
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
