#pragma once

#include <memory>

#include "../machine/machine_op_code.h"
#include "../asm_code/asm_code_translation_state.h"

#include "ast.h"

namespace mach::parsing{
	enum class header_item_type{
		stack,
		global,
	};

	MACH_AST_DECLARE_SINGLE_WPOS(asm_uninitialized_value, char)
	MACH_AST_DECLARE_SINGLE_WPOS(asm_position_placeholder, char)

	MACH_AST_DECLARE_SINGLE_WPOS(asm_integral_value, __int64)
	MACH_AST_DECLARE_SINGLE_WPOS(asm_float_value, long double)

	MACH_AST_DECLARE_SINGLE_WPOS(asm_char_value, char)
	MACH_AST_DECLARE_SINGLE_WPOS(asm_string_value, std::string)

	MACH_AST_DECLARE_SINGLE_WPOS(asm_section, asm_code::translation_state::section_type)

	MACH_AST_DECLARE_SINGLE_WPOS(asm_stack, unsigned __int64)
	MACH_AST_DECLARE_SINGLE_WPOS(asm_global, MACH_AST_NAME(asm_identifier))

	MACH_AST_DECLARE_SINGLE_WPOS(asm_label, boost::optional<char>, MACH_AST_NAME(mach_identifier))
	MACH_AST_DECLARE_SINGLE_WPOS(asm_identifier, std::vector<MACH_AST_NAME(mach_identifier)>)

	MACH_AST_DECLARE_PAIR_WPOS(asm_offset_item, machine::op_offset_operator, MACH_AST_VARIANT(MACH_AST_NAME(asm_integral_value), MACH_AST_NAME(asm_identifier), MACH_AST_NAME(asm_position_placeholder)))
	MACH_AST_DECLARE_PAIR_WPOS(asm_offset, MACH_AST_VARIANT(MACH_AST_NAME(asm_identifier), MACH_AST_NAME(asm_position_placeholder)), std::vector<MACH_AST_NAME(asm_offset_item)>)

	MACH_AST_DECLARE_SINGLE_WPOS(asm_offset_explicit, MACH_AST_NAME(asm_offset))
	MACH_AST_DECLARE_PAIR_WPOS(asm_memory, boost::optional<machine::op_operand_size>, MACH_AST_VARIANT(MACH_AST_NAME(asm_integral_value), MACH_AST_NAME(asm_identifier), MACH_AST_NAME(asm_offset), MACH_AST_NAME(asm_offset_explicit)))

	MACH_AST_DECLARE_PAIR_WPOS(asm_instruction, machine::op_code, std::vector<MACH_AST_VARIANT(
		MACH_AST_NAME(asm_integral_value),
		MACH_AST_NAME(asm_float_value),
		MACH_AST_NAME(asm_offset_explicit),
		MACH_AST_NAME(asm_char_value),
		MACH_AST_NAME(asm_string_value),
		MACH_AST_NAME(asm_identifier),
		MACH_AST_NAME(asm_memory)
	)>)
	MACH_AST_DECLARE_PAIR_WPOS(asm_times_instruction, unsigned __int64, MACH_AST_NAME(asm_instruction))

	MACH_AST_DECLARE_SINGLE_WPOS(asm_db, std::vector<MACH_AST_VARIANT(MACH_AST_NAME(asm_uninitialized_value), __int8, MACH_AST_NAME(asm_char_value), MACH_AST_NAME(asm_string_value))>)
	MACH_AST_DECLARE_SINGLE_WPOS(asm_dw, std::vector<MACH_AST_VARIANT(MACH_AST_NAME(asm_uninitialized_value), __int16)>)
	MACH_AST_DECLARE_SINGLE_WPOS(asm_dd, std::vector<MACH_AST_VARIANT(MACH_AST_NAME(asm_uninitialized_value), __int32, float)>)
	MACH_AST_DECLARE_SINGLE_WPOS(asm_dq, std::vector<MACH_AST_VARIANT(MACH_AST_NAME(asm_uninitialized_value), __int64, long double)>)

	MACH_AST_DECLARE_SINGLE_WPOS(asm_dz, unsigned __int64)
	MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(asm_ds, MACH_AST_NAME(asm_char_value), MACH_AST_NAME(asm_string_value))

	MACH_AST_DECLARE_PAIR_WPOS(asm_decl, MACH_AST_NAME(mach_identifier), MACH_AST_VARIANT(MACH_AST_NAME(asm_db), MACH_AST_NAME(asm_dw), MACH_AST_NAME(asm_dd), MACH_AST_NAME(asm_dq), MACH_AST_NAME(asm_dz), MACH_AST_NAME(asm_ds)))
	MACH_AST_DECLARE_PAIR_WPOS(asm_equ, MACH_AST_NAME(mach_identifier), MACH_AST_NAME(asm_offset))

	MACH_AST_DECLARE_SINGLE_WPOS(asm_program, std::vector<MACH_AST_VARIANT(
		MACH_AST_NAME(asm_section),
		MACH_AST_NAME(asm_stack),
		MACH_AST_NAME(asm_global),
		MACH_AST_NAME(asm_label),
		MACH_AST_NAME(asm_instruction),
		MACH_AST_NAME(asm_times_instruction),
		MACH_AST_NAME(asm_db),
		MACH_AST_NAME(asm_dw),
		MACH_AST_NAME(asm_dd),
		MACH_AST_NAME(asm_dq),
		MACH_AST_NAME(asm_dz),
		MACH_AST_NAME(asm_ds),
		MACH_AST_NAME(asm_decl),
		MACH_AST_NAME(asm_equ)
	)>)

	class MACH_AST_NAME(asm_operand_visitor){
	public:
		using instruction_operand_type = std::shared_ptr<asm_code::instruction_operand>;

		MACH_AST_NAME(asm_operand_visitor)(asm_code::translation_state &state)
			: state_(&state){}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_uninitialized_value) &ast) const{
			return std::make_shared<asm_code::uninitialized_instruction_operand>();
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_position_placeholder) &ast) const{
			return std::make_shared<asm_code::placeholder_instruction_operand>();
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_integral_value) &ast) const{
			return std::make_shared<asm_code::immediate_instruction_operand>(ast.value);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_float_value) &ast) const{
			return std::make_shared<asm_code::immediate_instruction_operand>(ast.value);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(mach_identifier) &ast) const{
			return std::make_shared<asm_code::label_ref_instruction_operand>(ast.value);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_identifier) &ast) const{
			std::string name;
			for (auto &item : ast.value){
				if (name.empty())
					name = item.value;
				else//Append
					name += ("." + item.value);
			}

			return std::make_shared<asm_code::label_ref_instruction_operand>(name);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_offset) &ast) const{
			return nullptr;
		}

	private:
		asm_code::translation_state *state_;
	};

	class MACH_AST_NAME(asm_statement_visitor){
	public:
		MACH_AST_NAME(asm_statement_visitor)(asm_code::translation_state &state)
			: state_(&state){}

		void operator ()(const MACH_AST_NAME(asm_instruction) &ast) const{

		}

	private:
		asm_code::translation_state *state_;
	};
}
