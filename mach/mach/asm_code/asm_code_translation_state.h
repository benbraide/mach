#pragma once

#include <map>
#include <list>
#include <variant>
#include <unordered_map>

#include "../asm_code/asm_code_instruction.h"

namespace mach::asm_code{
	enum class translation_error_code{
		nil,
		no_active_section,
		duplicate_label,
		bad_instruction_operand,
		unrecognized_instruction,
	};

	class translation_label{
	public:
		using entry_type = std::variant<instruction *, translation_label *>;

		explicit translation_label(const std::string &name, translation_label *parent = nullptr);

		translation_label *add_label(const std::string &name);

		void add_instruction(std::shared_ptr<instruction> value);

		const std::string &get_name() const;

		const std::string &get_qname() const;

		translation_label *get_parent() const;

		translation_label *find_label(const std::string &name) const;

		translation_label *find_qlabel(const std::string &qname) const;

	private:
		std::string name_;
		std::string qname_;
		translation_label *parent_;

		std::list<entry_type> entries_;
		std::list<std::shared_ptr<instruction>> instructions_;
		std::unordered_map<std::string, std::shared_ptr<translation_label>> labels_;
	};

	class translation_state;

	class translation_section{
	public:
		enum class id_type{
			header,
			rodata,
			data,
			text,
		};

		translation_section(id_type id, translation_state &state);

		translation_label *add_label(const std::string &name, bool nested);

		translation_label *add_label(const std::string &name, translation_label *parent);

		void add_instruction(std::shared_ptr<instruction> value);

		id_type get_id() const;

		translation_state *get_state() const;

		void update_offset(unsigned __int64 value);

		unsigned __int64 get_offset() const;

		translation_label *get_current_label() const;

		translation_label *find_label(const std::string &name) const;

		translation_label *find_qlabel(const std::string &qname) const;

	private:
		id_type id_;
		translation_state *state_;

		unsigned __int64 offset_ = 0;
		translation_label *current_label_ = nullptr;

		std::list<translation_label::entry_type> entries_;
		std::list<std::shared_ptr<instruction>> instructions_;
		std::unordered_map<std::string, std::shared_ptr<translation_label>> labels_;

		std::unordered_map<std::string, unsigned __int64> label_offsets_;
		std::unordered_map<std::string, std::list<label_ref_instruction_operand *>> label_refs_;
	};

	class translation_state{
	public:
		using section_type = translation_section::id_type;

		machine::register_table &get_reg_table();

		void update_offsets(unsigned __int64 start_offset);

		translation_label *add_label(const std::string &name, bool nested);

		translation_label *add_label(const std::string &name, translation_label *parent);

		void add_instruction(std::shared_ptr<instruction> value);

		translation_section *set_current_section(section_type id);

		translation_section *get_current_section() const;

		translation_label *get_current_label() const;

		translation_label *find_qlabel(const std::string &qname) const;

		void enter_data_instruction();

		void leave_data_instruction();

		bool is_inside_data_instruction() const;

	private:
		machine::register_table reg_table_;
		translation_section *current_section_ = nullptr;
		std::map<section_type, std::shared_ptr<translation_section>> sections_;
		bool is_inside_data_instruction_ = false;
	};
}
