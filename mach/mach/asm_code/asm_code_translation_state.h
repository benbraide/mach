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
	};

	class translation_label{
	public:
		using entry_type = std::variant<std::shared_ptr<instruction>, translation_label *>;

		explicit translation_label(const std::string &name, translation_label *parent = nullptr);

		translation_label *add_label(const std::string &name);

		void add_instruction(std::shared_ptr<instruction> value);

		const std::string &get_name() const;

		const std::string &get_qname() const;

		translation_label *get_parent() const;

		translation_label *find_label(const std::string &name) const;

	private:
		std::string name_;
		std::string qname_;
		translation_label *parent_;

		std::list<entry_type> entries_;
		std::unordered_map<std::string, std::shared_ptr<translation_label>> labels_;
	};

	class translation_section{
	public:
		enum class id_type{
			header,
			rodata,
			data,
			text,
		};

		explicit translation_section(id_type id);

		translation_label *add_label(const std::string &name, bool nested);

		void add_instruction(std::shared_ptr<instruction> value);

		id_type get_id() const;

		void update_offset(unsigned __int64 value);

		translation_label *get_current_label() const;

		translation_label *find_label(const std::string &name) const;

	private:
		id_type id_;
		unsigned __int64 offset_ = 0;
		translation_label *current_label_ = nullptr;

		std::list<translation_label::entry_type> entries_;
		std::unordered_map<std::string, std::shared_ptr<translation_label>> labels_;

		std::unordered_map<std::string, unsigned __int64> label_offsets_;
		std::unordered_map<std::string, std::list<label_ref_instruction_operand *>> label_refs_;
	};

	class translation_state{
	public:
		using section_type = translation_section::id_type;

		translation_label *add_label(const std::string &name, bool nested);

		void add_instruction(std::shared_ptr<instruction> value);

		translation_section *set_current_section(section_type id);

		translation_section *get_current_section() const;

	private:
		translation_section *current_section_ = nullptr;
		std::map<section_type, std::shared_ptr<translation_section>> sections_;
	};
}
