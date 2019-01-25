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
	entries_.push_back(value);
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

mach::asm_code::translation_section::translation_section(id_type id)
	: id_(id){}

mach::asm_code::translation_label *mach::asm_code::translation_section::add_label(const std::string &name, bool nested){
	if (!nested || current_label_ == nullptr){
		auto label = std::make_shared<translation_label>(name);
		entries_.push_back(label.get());
		current_label_ = (labels_[name] = label).get();
	}
	else//Nest inside current label
		current_label_ =  current_label_->add_label(name);

	label_offsets_[current_label_->get_qname()] = offset_;
	return current_label_;
}

void mach::asm_code::translation_section::add_instruction(std::shared_ptr<instruction> value){
	if (current_label_ == nullptr)
		entries_.push_back(value);
	else//Use current label
		current_label_->add_instruction(value);

	auto meta_size = value->get_meta_encoded_size();
	for (auto op : value->get_operands()){//Check for label references
		if (auto placeholder = dynamic_cast<placeholder_instruction_operand *>(op.get()); placeholder != nullptr){
			placeholder->set_data(offset_ + meta_size);
			label_refs_[""].push_back(placeholder);
		}
		else if (auto label_ref = dynamic_cast<label_ref_instruction_operand *>(op.get()); label_ref != nullptr)
			label_refs_[label_ref->get_name()].push_back(label_ref);

		meta_size += op->get_encoded_size(value->get_size_type());
	}

	offset_ += meta_size;
}

mach::asm_code::translation_section::id_type mach::asm_code::translation_section::get_id() const{
	return id_;
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
			item->set_data(item->get_data_as<unsigned __int64>() + item);
	}
}

mach::asm_code::translation_label *mach::asm_code::translation_section::get_current_label() const{
	return current_label_;
}

mach::asm_code::translation_label *mach::asm_code::translation_section::find_label(const std::string &name) const{
	auto iter = labels_.find(name);
	return ((iter == labels_.end()) ? nullptr : iter->second.get());
}

mach::asm_code::translation_label *mach::asm_code::translation_state::add_label(const std::string &name, bool nested){
	if (current_section_ == nullptr)
		throw translation_error_code::no_active_section;

	return current_section_->add_label(name, nested);
}

void mach::asm_code::translation_state::add_instruction(std::shared_ptr<instruction> value){
	if (current_section_ == nullptr)
		throw translation_error_code::no_active_section;

	value->validate_operands();
	current_section_->add_instruction(value);
}

mach::asm_code::translation_section *mach::asm_code::translation_state::set_current_section(section_type id){
	if (auto it = sections_.find(id); it == sections_.end())
		current_section_ = (sections_[id] = std::make_shared<translation_section>(id)).get();
	else//Already exists
		current_section_ = it->second.get();

	return current_section_;
}

mach::asm_code::translation_section *mach::asm_code::translation_state::get_current_section() const{
	return current_section_;
}
