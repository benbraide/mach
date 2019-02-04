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
		label_not_found,
		duplicate_label,
		bad_instruction_operand,
		unrecognized_instruction,
	};

	class translation_section;
	class translation_state;

	class translation_label{
	public:
		translation_label(const std::string &name, unsigned __int64 offset);

		virtual const std::string &get_name() const;

		virtual unsigned __int64 get_offset() const;

	protected:
		friend class translation_section;

		std::string name_;
		unsigned __int64 offset_ = 0u;
	};

	class translation_equ_label : public translation_label{
	public:
		translation_equ_label(const std::string &name, unsigned __int64 offset, std::shared_ptr<instruction_operand> expression);

		virtual unsigned __int64 get_offset() const override;

	protected:
		friend class translation_section;

		std::shared_ptr<instruction_operand> expression_;
	};

	class translation_section{
	public:
		using entry_type = std::variant<instruction *, translation_label *>;

		enum class id_type{
			header,
			rodata,
			data,
			text,
		};

		translation_section(id_type id, translation_state &state);

		translation_label *add_label(const std::string &name);

		void add_equ(const std::string &name, std::shared_ptr<instruction_operand> op);

		void add_instruction(std::shared_ptr<instruction> value);

		id_type get_id() const;

		translation_state *get_state() const;

		void resolve(unsigned __int64 start_address);

		unsigned __int64 get_offset() const;

		translation_label *find_label(const std::string &name) const;

	private:
		id_type id_;
		translation_state *state_;
		unsigned __int64 offset_ = 0;

		std::list<entry_type> entries_;
		std::list<std::shared_ptr<instruction>> instructions_;

		std::unordered_map<std::string, std::shared_ptr<translation_label>> labels_;
		std::unordered_map<std::string, std::list<label_ref_instruction_operand *>> label_refs_;
		std::unordered_map<std::string, std::list<label_ref_instruction_operand *>> equ_label_refs_;

		std::list<placeholder_instruction_operand *> placeholders_;
	};

	class translation_state{
	public:
		using section_type = translation_section::id_type;

		machine::register_table &get_reg_table();

		std::size_t resolve(unsigned __int64 start_offset);

		translation_label *add_label(const std::string &name);

		void add_equ(const std::string &name, std::shared_ptr<instruction_operand> op);

		void add_instruction(std::shared_ptr<instruction> value);

		translation_section *set_current_section(section_type id);

		translation_section *get_current_section() const;

		translation_label *find_label(const std::string &name) const;

		void enter_data_instruction();

		void leave_data_instruction();

		bool is_inside_data_instruction() const;

		void set_stack_size(std::size_t value);

		std::size_t get_stack_size() const;

		void set_entry(const std::string &value);

		const std::string &get_entry() const;

		unsigned __int64 get_entry_address() const;

	private:
		machine::register_table reg_table_;
		translation_section *current_section_ = nullptr;
		std::map<section_type, std::shared_ptr<translation_section>> sections_;
		bool is_inside_data_instruction_ = false;
		std::size_t stack_size_ = 0u;
		std::string entry_;
	};
}
