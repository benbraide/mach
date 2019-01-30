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

	enum class decl_type{
		db,
		dw,
		dd,
		dq,
	};

	MACH_AST_DECLARE_SINGLE_WPOS(asm_uninitialized_value, char);

	MACH_AST_DECLARE_SINGLE_WPOS(asm_integral_value, __int64);
	MACH_AST_DECLARE_SINGLE_WPOS(asm_float_value, long double);

	MACH_AST_DECLARE_SINGLE_WPOS(asm_char_value, char);
	MACH_AST_DECLARE_SINGLE_WPOS(asm_string_value, std::string);

	MACH_AST_DECLARE_PAIR_WPOS(asm_identifier, MACH_AST_NAME(mach_identifier), std::vector<MACH_AST_NAME(mach_identifier)>);

	MACH_AST_DECLARE_SINGLE_WPOS(asm_section, asm_code::translation_state::section_type);
	MACH_AST_DECLARE_PAIR_WPOS(asm_label, std::string, MACH_AST_NAME(mach_identifier));

	MACH_AST_DECLARE_SINGLE_WPOS(asm_stack, unsigned __int64);
	MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(asm_global, MACH_AST_NAME(mach_identifier), MACH_AST_NAME(asm_identifier));

	MACH_AST_DECLARE_NAME(decl_sum_expression);

	MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(decl_expression_term, MACH_AST_NAME(asm_integral_value), MACH_AST_NAME(asm_float_value), MACH_AST_NAME(mach_identifier), MACH_AST_NAME(asm_identifier));
	MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(decl_grouped_expression, MACH_AST_NAME(decl_expression_term), MACH_AST_FORWARD_NAME(decl_sum_expression));
	MACH_AST_DECLARE_TRIO_WPOS(
		decl_product_expression,
		MACH_AST_VARIANT(MACH_AST_NAME(decl_expression_term), MACH_AST_NAME(decl_grouped_expression)),
		char,
		MACH_AST_VARIANT(MACH_AST_NAME(decl_expression_term), MACH_AST_FORWARD_NAME(decl_product_expression))
	);

	MACH_AST_DECLARE_TRIO_WPOS(
		decl_sum_expression,
		MACH_AST_VARIANT(MACH_AST_NAME(decl_expression_term), MACH_AST_NAME(decl_product_expression)),
		char,
		MACH_AST_VARIANT(MACH_AST_NAME(decl_expression_term), MACH_AST_FORWARD_NAME(decl_sum_expression))
	);

	MACH_AST_DECLARE_PAIR_WPOS(asm_offset_item, machine::op_offset_operator, MACH_AST_VARIANT(MACH_AST_NAME(asm_integral_value), MACH_AST_NAME(mach_identifier), MACH_AST_NAME(asm_identifier)));
	MACH_AST_DECLARE_TRIO_WPOS(asm_offset, bool, MACH_AST_VARIANT(MACH_AST_NAME(mach_identifier), MACH_AST_NAME(asm_identifier)), std::vector<MACH_AST_NAME(asm_offset_item)>);
	MACH_AST_DECLARE_PAIR_WPOS(asm_memory, boost::optional<machine::op_operand_size>, MACH_AST_VARIANT(MACH_AST_NAME(asm_integral_value), MACH_AST_NAME(mach_identifier), MACH_AST_NAME(asm_identifier), MACH_AST_NAME(asm_offset)));

	MACH_AST_DECLARE_SINGLE_WPOS(asm_no_arg_instruction, machine::op_code);
	MACH_AST_DECLARE_PAIR_WPOS(asm_single_arg_instruction, machine::op_code, MACH_AST_VARIANT(
		MACH_AST_NAME(asm_integral_value),
		MACH_AST_NAME(asm_float_value),
		MACH_AST_NAME(asm_offset),
		MACH_AST_NAME(asm_char_value),
		MACH_AST_NAME(asm_string_value),
		MACH_AST_NAME(mach_identifier),
		MACH_AST_NAME(asm_identifier),
		MACH_AST_NAME(asm_memory)
	));

	MACH_AST_DECLARE_TRIO_WPOS(asm_pair_args_instruction, machine::op_code, MACH_AST_VARIANT(
		MACH_AST_NAME(asm_integral_value),
		MACH_AST_NAME(asm_float_value),
		MACH_AST_NAME(asm_offset),
		MACH_AST_NAME(asm_char_value),
		MACH_AST_NAME(asm_string_value),
		MACH_AST_NAME(mach_identifier),
		MACH_AST_NAME(asm_identifier),
		MACH_AST_NAME(asm_memory)
	), MACH_AST_VARIANT(
		MACH_AST_NAME(asm_integral_value),
		MACH_AST_NAME(asm_float_value),
		MACH_AST_NAME(asm_offset),
		MACH_AST_NAME(asm_char_value),
		MACH_AST_NAME(asm_string_value),
		MACH_AST_NAME(mach_identifier),
		MACH_AST_NAME(asm_identifier),
		MACH_AST_NAME(asm_memory)
	));

	MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(
		asm_instruction,
		MACH_AST_NAME(asm_no_arg_instruction),
		MACH_AST_NAME(asm_single_arg_instruction),
		MACH_AST_NAME(asm_pair_args_instruction)
	);

	MACH_AST_DECLARE_PAIR_WPOS(asm_times_instruction, unsigned __int64, MACH_AST_NAME(asm_instruction));
	MACH_AST_DECLARE_PAIR_WPOS(
		asm_db,
		decl_type,
		std::vector<MACH_AST_VARIANT(
			MACH_AST_NAME(asm_uninitialized_value),
			MACH_AST_NAME(asm_integral_value),
			MACH_AST_NAME(asm_float_value),
			MACH_AST_NAME(asm_char_value),
			MACH_AST_NAME(asm_string_value),
			MACH_AST_NAME(mach_identifier),
			MACH_AST_NAME(asm_identifier),
			MACH_AST_NAME(decl_sum_expression),
			MACH_AST_NAME(decl_product_expression),
			MACH_AST_NAME(decl_grouped_expression)
		)>
	);

	MACH_AST_DECLARE_SINGLE_WPOS(asm_dz, unsigned __int64);
	MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(asm_ds, MACH_AST_NAME(asm_char_value), MACH_AST_NAME(asm_string_value));

	MACH_AST_DECLARE_PAIR_WPOS(asm_decl, MACH_AST_NAME(mach_identifier), MACH_AST_VARIANT(MACH_AST_NAME(asm_db), MACH_AST_NAME(asm_dz), MACH_AST_NAME(asm_ds)));
	MACH_AST_DECLARE_PAIR_WPOS(asm_equ, MACH_AST_NAME(mach_identifier), MACH_AST_VARIANT(MACH_AST_NAME(asm_integral_value), MACH_AST_NAME(mach_identifier), MACH_AST_NAME(asm_identifier), MACH_AST_NAME(asm_offset)));

	MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(
		asm_statement,
		MACH_AST_NAME(asm_section),
		MACH_AST_NAME(asm_stack),
		MACH_AST_NAME(asm_global),
		MACH_AST_NAME(asm_label),
		MACH_AST_NAME(asm_instruction),
		MACH_AST_NAME(asm_times_instruction),
		MACH_AST_NAME(asm_db),
		MACH_AST_NAME(asm_dz),
		MACH_AST_NAME(asm_ds),
		MACH_AST_NAME(asm_decl),
		MACH_AST_NAME(asm_equ)
	);

	MACH_AST_DECLARE_SINGLE_WPOS(asm_program, std::vector<MACH_AST_NAME(asm_statement)>);

	class MACH_AST_NAME(asm_operand_visitor){
	public:
		using instruction_operand_type = std::shared_ptr<asm_code::instruction_operand>;

		MACH_AST_NAME(asm_operand_visitor)(asm_code::translation_state &state)
			: state_(&state){}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_uninitialized_value) &ast) const{
			if (!state_->is_inside_data_instruction())
				throw asm_code::translation_error_code::bad_instruction_operand;
			return std::make_shared<asm_code::uninitialized_instruction_operand>();
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_integral_value) &ast) const{
			return std::make_shared<asm_code::immediate_instruction_operand>(ast.MACH_AST_SINGLE_FIELD_NAME, state_->is_inside_data_instruction());
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_float_value) &ast) const{
			return std::make_shared<asm_code::immediate_instruction_operand>(ast.MACH_AST_SINGLE_FIELD_NAME, state_->is_inside_data_instruction());
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_char_value) &ast) const{
			return std::make_shared<asm_code::immediate_instruction_operand>(ast.MACH_AST_SINGLE_FIELD_NAME, state_->is_inside_data_instruction(), asm_code::immediate_instruction_operand::held_value_type::char_);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_string_value) &ast) const{
			if (!state_->is_inside_data_instruction())
				throw asm_code::translation_error_code::bad_instruction_operand;
			return std::make_shared<asm_code::immediate_instruction_operand>(ast.MACH_AST_SINGLE_FIELD_NAME);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(mach_identifier) &ast) const{
			//return std::make_shared<asm_code::register_instruction_operand>(*state_->get_reg_table().find(ast.MACH_AST_SINGLE_FIELD_NAME.MACH_AST_SINGLE_FIELD_NAME, false));
			if (ast.MACH_AST_SINGLE_FIELD_NAME.size() == 1u && ast.MACH_AST_SINGLE_FIELD_NAME[0] == '$')
				return std::make_shared<asm_code::placeholder_instruction_operand>(state_->is_inside_data_instruction());
			return std::make_shared<asm_code::label_ref_instruction_operand>(ast.MACH_AST_SINGLE_FIELD_NAME, state_->is_inside_data_instruction());
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_identifier) &ast) const{
			std::string name = ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME.MACH_AST_SINGLE_FIELD_NAME;
			for (auto &item : ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME)
				name += ("." + item.MACH_AST_SINGLE_FIELD_NAME);

			return std::make_shared<asm_code::label_ref_instruction_operand>(name, state_->is_inside_data_instruction());
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_memory) &ast) const{
			return nullptr;
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(asm_offset) &ast) const{
			std::vector<asm_code::offset_instruction_operand::part_info> parts;

			parts.reserve(ast.MACH_AST_MULTIPLE_THIRD_FIELD_NAME.size() + 1u);
			parts.push_back(asm_code::offset_instruction_operand::part_info{
				machine::op_offset_operator::plus,
				boost::apply_visitor(*this, ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME)
			});

			for (auto &part : ast.MACH_AST_MULTIPLE_THIRD_FIELD_NAME){
				parts.push_back(asm_code::offset_instruction_operand::part_info{
					part.MACH_AST_MULTIPLE_FIRST_FIELD_NAME,
					boost::apply_visitor(*this, part.MACH_AST_MULTIPLE_SECOND_FIELD_NAME)
				});
			}

			return std::make_shared<asm_code::offset_instruction_operand>(std::move(parts), ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(decl_expression_term) &ast) const{
			return boost::apply_visitor(*this, ast.MACH_AST_SINGLE_FIELD_NAME);
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(decl_sum_expression) &ast) const{
			return std::make_shared<asm_code::expression_instruction_operand>(ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME, boost::apply_visitor(*this, ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME), boost::apply_visitor(*this, ast.MACH_AST_MULTIPLE_THIRD_FIELD_NAME));
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(decl_product_expression) &ast) const{
			return std::make_shared<asm_code::expression_instruction_operand>(ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME, boost::apply_visitor(*this, ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME), boost::apply_visitor(*this, ast.MACH_AST_MULTIPLE_THIRD_FIELD_NAME));
		}

		instruction_operand_type operator ()(const MACH_AST_NAME(decl_grouped_expression) &ast) const{
			return boost::apply_visitor(*this, ast.MACH_AST_SINGLE_FIELD_NAME);
		}

	private:
		asm_code::translation_state *state_;
	};

	class MACH_AST_NAME(asm_statement_visitor){
	public:
		MACH_AST_NAME(asm_statement_visitor)(asm_code::translation_state &state)
			: state_(&state){}

		void operator ()(const MACH_AST_NAME(asm_program) &ast) const{
			for (auto &statement : ast.MACH_AST_SINGLE_FIELD_NAME)
				boost::apply_visitor(*this, statement.MACH_AST_SINGLE_FIELD_NAME);
		}

		void operator ()(const MACH_AST_NAME(asm_statement) &ast) const{
			boost::apply_visitor(*this, ast.MACH_AST_SINGLE_FIELD_NAME);
		}

		void operator ()(const MACH_AST_NAME(asm_times_instruction) &ast) const{
			for (auto i = 0ui64; i < ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME; ++i)
				boost::apply_visitor(*this, ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME.MACH_AST_SINGLE_FIELD_NAME);
		}

		void operator ()(const MACH_AST_NAME(asm_instruction) &ast) const{
			boost::apply_visitor(*this, ast.MACH_AST_SINGLE_FIELD_NAME);
		}

		void operator ()(const MACH_AST_NAME(asm_no_arg_instruction) &ast) const{
			std::vector<std::shared_ptr<asm_code::instruction_operand>> operands;
			state_->add_instruction(create_instruction(ast.MACH_AST_SINGLE_FIELD_NAME, operands));
		}

		void operator ()(const MACH_AST_NAME(asm_single_arg_instruction) &ast) const{
			/*std::vector<std::shared_ptr<asm_code::instruction_operand>> operands;
			state_->add_instruction(create_instruction(ast.MACH_AST_SINGLE_FIELD_NAME, operands));*/
		}

		void operator ()(const MACH_AST_NAME(asm_pair_args_instruction) &ast) const{
			/*std::vector<std::shared_ptr<asm_code::instruction_operand>> operands;
			state_->add_instruction(create_instruction(ast.MACH_AST_SINGLE_FIELD_NAME, operands));*/
		}

		/*void operator ()(const MACH_AST_NAME(asm_instruction) &ast) const{
			std::vector<std::shared_ptr<asm_code::instruction_operand>> operands;
			operands.reserve(ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME.size());

			for (auto &op : ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME)
				operands.push_back(boost::apply_visitor(MACH_AST_NAME(asm_operand_visitor)(*state_), op));

			state_->add_instruction(create_instruction(ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME, operands));
		}*/

		void operator ()(const MACH_AST_NAME(asm_section) &ast) const{
			state_->set_current_section(ast.MACH_AST_SINGLE_FIELD_NAME);
		}

		void operator ()(const MACH_AST_NAME(asm_stack) &ast) const{

		}

		void operator ()(const MACH_AST_NAME(asm_global) &ast) const{

		}

		void operator ()(const MACH_AST_NAME(asm_label) &ast) const{
			if (ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME.empty() || ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME[0] != '.'){
				asm_code::translation_label *target_label;
				if (ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME[0] == '^'){
					auto current_label = state_->get_current_label();
					target_label = ((current_label == nullptr) ? nullptr : current_label->get_parent());

					for (auto i = ast.MACH_AST_MULTIPLE_FIRST_FIELD_NAME.size(); 0u < i && target_label != nullptr; --i)
						target_label = target_label->get_parent();
				}
				else//Root
					target_label = nullptr;

				state_->add_label(ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME.MACH_AST_SINGLE_FIELD_NAME, target_label);
			}
			else//Nested label
				state_->add_label(ast.MACH_AST_MULTIPLE_SECOND_FIELD_NAME.MACH_AST_SINGLE_FIELD_NAME, true);
		}

		void operator ()(const MACH_AST_NAME(asm_db) &ast) const{

		}

		void operator ()(const MACH_AST_NAME(asm_dz) &ast) const{

		}

		void operator ()(const MACH_AST_NAME(asm_ds) &ast) const{

		}

		void operator ()(const MACH_AST_NAME(asm_decl) &ast) const{

		}

		void operator ()(const MACH_AST_NAME(asm_equ) &ast) const{

		}

		static std::shared_ptr<asm_code::instruction> create_instruction(machine::op_code op_code, std::vector<std::shared_ptr<asm_code::instruction_operand>> &operands){
			switch (op_code){
			case machine::op_code::nop:
				return std::make_shared<asm_code::nop_instruction>(std::move(operands));
			case machine::op_code::push:
				return nullptr;
			case machine::op_code::pop:
				return nullptr;
			case machine::op_code::mov:
				return std::make_shared<asm_code::mov_instruction>(std::move(operands));
			case machine::op_code::lea:
				return nullptr;
			case machine::op_code::syscall:
				return std::make_shared<asm_code::syscall_instruction>(std::move(operands));
			case machine::op_code::call:
				return nullptr;
			case machine::op_code::enter:
				return nullptr;
			case machine::op_code::leave:
				return std::make_shared<asm_code::leave_instruction>(std::move(operands));
			case machine::op_code::ret:
				return std::make_shared<asm_code::ret_instruction>(std::move(operands));
			case machine::op_code::jmp:
				return nullptr;
			case machine::op_code::jz:
				return nullptr;
			case machine::op_code::jnz:
				return nullptr;
			case machine::op_code::je:
				return nullptr;
			case machine::op_code::jne:
				return nullptr;
			case machine::op_code::jl:
				return nullptr;
			case machine::op_code::jnl:
				return nullptr;
			case machine::op_code::jle:
				return nullptr;
			case machine::op_code::jnle:
				return nullptr;
			case machine::op_code::jg:
				return nullptr;
			case machine::op_code::jng:
				return nullptr;
			case machine::op_code::jge:
				return nullptr;
			case machine::op_code::jnge:
				return nullptr;
			case machine::op_code::setz:
				return nullptr;
			case machine::op_code::setnz:
				return nullptr;
			case machine::op_code::sete:
				return nullptr;
			case machine::op_code::setne:
				return nullptr;
			case machine::op_code::setl:
				return nullptr;
			case machine::op_code::setnl:
				return nullptr;
			case machine::op_code::setle:
				return nullptr;
			case machine::op_code::setnle:
				return nullptr;
			case machine::op_code::setg:
				return nullptr;
			case machine::op_code::setng:
				return nullptr;
			case machine::op_code::setge:
				return nullptr;
			case machine::op_code::setnge:
				return nullptr;
			case machine::op_code::loop:
				return nullptr;
			case machine::op_code::inc:
				return nullptr;
			case machine::op_code::dec:
				return nullptr;
			case machine::op_code::add:
				return nullptr;
			case machine::op_code::sub:
				return nullptr;
			case machine::op_code::mult:
				return nullptr;
			case machine::op_code::div:
				return nullptr;
			case machine::op_code::mod:
				return nullptr;
			case machine::op_code::and_:
				return nullptr;
			case machine::op_code::xor_:
				return nullptr;
			case machine::op_code::or_:
				return nullptr;
			case machine::op_code::sal:
				return nullptr;
			case machine::op_code::sar:
				return nullptr;
			case machine::op_code::test:
				return nullptr;
			case machine::op_code::not_:
				return nullptr;
			case machine::op_code::cmp:
				return nullptr;
			default:
				throw asm_code::translation_error_code::unrecognized_instruction;
				break;
			}

			return nullptr;
		}

	private:
		asm_code::translation_state *state_;
	};
}

MACH_AST_ADAPT_SINGLE(asm_uninitialized_value)

MACH_AST_ADAPT_SINGLE(asm_integral_value)
MACH_AST_ADAPT_SINGLE(asm_float_value)

MACH_AST_ADAPT_SINGLE(asm_char_value)
MACH_AST_ADAPT_SINGLE(asm_string_value)

MACH_AST_ADAPT_SINGLE(asm_stack)
MACH_AST_ADAPT_SINGLE(asm_global)

MACH_AST_ADAPT_PAIR(asm_identifier)

MACH_AST_ADAPT_SINGLE(asm_section)
MACH_AST_ADAPT_PAIR(asm_label)

MACH_AST_ADAPT_SINGLE(decl_expression_term)
MACH_AST_ADAPT_SINGLE(decl_grouped_expression)
MACH_AST_ADAPT_TRIO(decl_product_expression)

MACH_AST_ADAPT_TRIO(decl_sum_expression)

MACH_AST_ADAPT_PAIR(asm_offset_item)
MACH_AST_ADAPT_TRIO(asm_offset)
MACH_AST_ADAPT_PAIR(asm_memory)

MACH_AST_ADAPT_SINGLE(asm_no_arg_instruction)
MACH_AST_ADAPT_PAIR(asm_single_arg_instruction)
MACH_AST_ADAPT_TRIO(asm_pair_args_instruction)

MACH_AST_ADAPT_SINGLE(asm_instruction)
MACH_AST_ADAPT_PAIR(asm_times_instruction)

MACH_AST_ADAPT_PAIR(asm_db)

MACH_AST_ADAPT_SINGLE(asm_dz)
MACH_AST_ADAPT_SINGLE(asm_ds)

MACH_AST_ADAPT_PAIR(asm_decl)
MACH_AST_ADAPT_PAIR(asm_equ)

MACH_AST_ADAPT_SINGLE(asm_statement)
MACH_AST_ADAPT_SINGLE(asm_program)
