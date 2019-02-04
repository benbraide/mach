#pragma once

#include "asm_ast.h"
#include "grammar.h"

namespace mach::parsing{
	struct MACH_GRAMMAR_TAG_NAME(asm_translation_state);

	MACH_GRAMMAR_ANNOTATOR_BEGIN(asm_register)
		auto &translation_state = MACH_GRAMMAR_ACTION_GET_TAGGED(asm_translation_state);
		if (translation_state.get_reg_table().find(std::string(MACH_GRAMMAR_ANNOTATOR_FIRST_ITERATOR_NAME, MACH_GRAMMAR_ANNOTATOR_LAST_ITERATOR_NAME), false) == nullptr)
			MACH_GRAMMAR_ANNOTATOR_FAIL;
		else//Identifier is a register
			MACH_GRAMMAR_ANNOTATOR_CALL_BASE;
	MACH_GRAMMAR_ANNOTATOR_END;

	MACH_GRAMMAR_ERROR_HANDLER_BEGIN(asm_program)
		MACH_GRAMMAR_ERROR_HANDLER_HANDLE_ERROR;
	MACH_GRAMMAR_ERROR_HANDLER_END;

	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_section, asm_code::translation_state::section_type)
		(".head", asm_code::translation_state::section_type::header)
		(".rodata", asm_code::translation_state::section_type::rodata)
		(".data", asm_code::translation_state::section_type::data)
		(".text", asm_code::translation_state::section_type::text)
	MACH_GRAMMAR_SYMBOLS_END(asm_section);

	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_type, machine::op_operand_size)
		("byte", machine::op_operand_size::byte)
		("word", machine::op_operand_size::word)
		("dword", machine::op_operand_size::dword)
		("qword", machine::op_operand_size::qword)
		("float", machine::op_operand_size::dfloat)
		("qfloat", machine::op_operand_size::qfloat)
	MACH_GRAMMAR_SYMBOLS_END(asm_type);
	
	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_decl_type, decl_type)
		("db", decl_type::db)
		("dw", decl_type::dw)
		("dd", decl_type::dd)
		("dq", decl_type::dq)
	MACH_GRAMMAR_SYMBOLS_END(asm_decl_type);

	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_no_arg_instruction, machine::op_code)
		("nop", machine::op_code::nop)
		("leave", machine::op_code::leave)
		("ret", machine::op_code::ret)
		("syscall", machine::op_code::syscall)
	MACH_GRAMMAR_SYMBOLS_END(asm_no_arg_instruction);

	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_std_instruction, machine::op_code)
		("mov", machine::op_code::mov)
		("cmp", machine::op_code::cmp)
		("test", machine::op_code::test)
	MACH_GRAMMAR_SYMBOLS_END(asm_std_instruction);
	
	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_jmp_instruction, machine::op_code)
		("call", machine::op_code::call)
		("jmp", machine::op_code::jmp)
		("jnz", machine::op_code::jnz)
		("je", machine::op_code::je)
		("jne", machine::op_code::jne)
		("jl", machine::op_code::jl)
		("jnl", machine::op_code::jnl)
		("jle", machine::op_code::jle)
		("jnle", machine::op_code::jnle)
		("jg", machine::op_code::jg)
		("jng", machine::op_code::jng)
		("jge", machine::op_code::jge)
		("jnge", machine::op_code::jnge)
	MACH_GRAMMAR_SYMBOLS_END(asm_jmp_instruction);
	
	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_set_instruction, machine::op_code)
		("setz", machine::op_code::setz)
		("setnz", machine::op_code::setnz)
		("sete", machine::op_code::sete)
		("setne", machine::op_code::setne)
		("setl", machine::op_code::setl)
		("setnl", machine::op_code::setnl)
		("setle", machine::op_code::setle)
		("setnle", machine::op_code::setnle)
		("setg", machine::op_code::setg)
		("setng", machine::op_code::setng)
		("setge", machine::op_code::setge)
		("setnge", machine::op_code::setnge)
		("loop", machine::op_code::loop)
	MACH_GRAMMAR_SYMBOLS_END(asm_set_instruction);
	
	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_stack_instruction, machine::op_code)
		("push", machine::op_code::push)
		("pop", machine::op_code::pop)
	MACH_GRAMMAR_SYMBOLS_END(asm_stack_instruction);
	
	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_unary_arith_instruction, machine::op_code)
		("not", machine::op_code::not_)
		("inc", machine::op_code::inc)
		("dec", machine::op_code::dec)
	MACH_GRAMMAR_SYMBOLS_END(asm_unary_arith_instruction);
	
	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_binary_arith_instruction, machine::op_code)
		("add", machine::op_code::add)
		("sub", machine::op_code::sub)
		("mult", machine::op_code::mult)
		("div", machine::op_code::div)
	MACH_GRAMMAR_SYMBOLS_END(asm_binary_arith_instruction);

	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_binary_int_arith_instruction, machine::op_code)
		("mod", machine::op_code::mod)
		("and", machine::op_code::and_)
		("xor", machine::op_code::xor_)
		("or", machine::op_code::or_)
		("sal", machine::op_code::sal)
		("sar", machine::op_code::sar)
	MACH_GRAMMAR_SYMBOLS_END(asm_binary_int_arith_instruction);

	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_offset_op, machine::op_offset_operator)
		("+", machine::op_offset_operator::plus)
		("-", machine::op_offset_operator::minus)
	MACH_GRAMMAR_SYMBOLS_END(asm_offset_op);
	
	MACH_GRAMMAR_SYMBOLS_BEGIN(asm_escaped_character, char)
		("0", '\0')
		("a", '\a')
		("b", '\b')
		("f", '\f')
		("n", '\n')
		("r", '\r')
		("t", '\v')
		("v", '\v')
		("\"", '"')
		("'", '\'')
	MACH_GRAMMAR_SYMBOLS_END(asm_escaped_character);

	MACH_GRAMMAR_DECLARE_RULE(asm_uninitialized_value);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_uninitialized_value) = helper::keyword(x3::char_('?'));

	mach_hex_parser<__int64> const asm_int64_hex = {};
	mach_hex_parser<__int32> const asm_int32_hex = {};
	mach_hex_parser<__int16> const asm_int16_hex = {};
	mach_hex_parser<__int8> const asm_int8_hex = {};

	mach_dec_parser<__int64> const asm_int64_dec = {};
	mach_dec_parser<__int32> const asm_int32_dec = {};
	mach_dec_parser<__int16> const asm_int16_dec = {};
	mach_dec_parser<__int8> const asm_int8_dec = {};

	mach_oct_parser<__int64> const asm_int64_oct = {};
	mach_oct_parser<__int32> const asm_int32_oct = {};
	mach_oct_parser<__int16> const asm_int16_oct = {};
	mach_oct_parser<__int8> const asm_int8_oct = {};

	mach_bin_parser<__int64> const asm_int64_bin = {};
	mach_bin_parser<__int32> const asm_int32_bin = {};
	mach_bin_parser<__int16> const asm_int16_bin = {};
	mach_bin_parser<__int8> const asm_int8_bin = {};

	MACH_GRAMMAR_DECLARE_RULE(asm_integral_value);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_integral_value) = (
		x3::lexeme["0x" > asm_int64_hex] |
		x3::lexeme[&x3::digit >> asm_int64_hex] >> helper::keyword(x3::no_case['h']) |
		x3::lexeme["0b" > asm_int64_bin] |
		x3::lexeme["0" >> asm_int64_oct] |
		(x3::lexeme[x3::int64] >> !x3::lit("."))
	);

	MACH_GRAMMAR_DECLARE_RULE(asm_float_value);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_float_value) = mach_long_double;

	MACH_GRAMMAR_DECLARE_RULE(asm_char_value);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_char_value) = ("'" >> ((x3::char_('\\') >> (('x' >> asm_int8_hex) | ('0' >> asm_int8_oct) | asm_int8_dec | MACH_GRAMMAR_SYMBOLS_NAME(asm_escaped_character) | x3::char_)) | ~x3::char_("'")) >> "'");

	MACH_GRAMMAR_DECLARE_RULE2(asm_expected_char_value, asm_char_value);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_expected_char_value) = ("'" >> ((x3::char_('\\') > (('x' >> asm_int8_hex) | ('b' >> asm_int8_bin) | ('0' >> asm_int8_oct) | asm_int8_dec | MACH_GRAMMAR_SYMBOLS_NAME(asm_escaped_character) | x3::char_)) | ~x3::char_("'")) > "'");

	MACH_GRAMMAR_DECLARE_RULE(asm_string_value);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_string_value) = ("'" > x3::lexeme[+((x3::char_('\\') >> (('x' >> asm_int8_hex) | ('b' >> asm_int8_bin) | ('0' >> asm_int8_oct) | asm_int8_dec | MACH_GRAMMAR_SYMBOLS_NAME(asm_escaped_character) | x3::char_)) | ~x3::char_("'"))] > "'");

	MACH_GRAMMAR_DECLARE_RULE4(asm_register, mach_identifier, asm_register);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_register) = helper::keyword(MACH_GRAMMAR_NAME(mach_identifier));

	MACH_GRAMMAR_DECLARE_RULE0(asm_not_identifier);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_not_identifier) = x3::omit[helper::keyword(
		x3::lit("section") | x3::lit("offset") | x3::lit("lea") | x3::lit("enter") | x3::lit("cnvt") | x3::lit("times") | x3::lit("equ") |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_type) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_decl_type) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_no_arg_instruction) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_std_instruction) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_jmp_instruction) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_set_instruction) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_stack_instruction) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_unary_arith_instruction) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_binary_arith_instruction) |
		MACH_GRAMMAR_SYMBOLS_NAME(asm_binary_int_arith_instruction) |
		MACH_GRAMMAR_NAME(asm_register)
	)];

	MACH_GRAMMAR_DECLARE_RULE2(asm_identifier_part, mach_identifier);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_identifier_part) = (!MACH_GRAMMAR_NAME(asm_not_identifier) >> MACH_GRAMMAR_NAME(mach_identifier));

	MACH_GRAMMAR_DECLARE_RULE(asm_identifier);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_identifier) = x3::lexeme[(MACH_GRAMMAR_NAME(asm_identifier_part) >> *('.' > MACH_GRAMMAR_NAME(asm_identifier_part)))];

	MACH_GRAMMAR_DECLARE_RULE(asm_section);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_section) = x3::no_case[(helper::keyword("section") > MACH_GRAMMAR_SYMBOLS_NAME(asm_section))];

	MACH_GRAMMAR_DECLARE_RULE(asm_label);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_label) = (x3::lexeme[MACH_GRAMMAR_NAME(asm_identifier) >> ":"]);

	MACH_GRAMMAR_DECLARE_RULE(asm_stack);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_stack) = (helper::keyword(x3::no_case[".stack"]) > x3::uint64);

	MACH_GRAMMAR_DECLARE_RULE(asm_global);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_global) = (helper::keyword(x3::no_case[".global"]) > MACH_GRAMMAR_NAME(asm_identifier));

	MACH_GRAMMAR_DECLARE_RULE(decl_expression_term);
	MACH_GRAMMAR_RULE_DEF_PREFIX(decl_expression_term) = (MACH_GRAMMAR_NAME(asm_float_value) | MACH_GRAMMAR_NAME(asm_integral_value) | MACH_GRAMMAR_NAME(asm_identifier));

	MACH_GRAMMAR_DECLARE_RULE(decl_sum_expression);

	MACH_GRAMMAR_DECLARE_RULE(decl_grouped_expression);
	MACH_GRAMMAR_RULE_DEF_PREFIX(decl_grouped_expression) = ('(' > (MACH_GRAMMAR_NAME(decl_sum_expression) | MACH_GRAMMAR_NAME(decl_expression_term)) > ')');

	MACH_GRAMMAR_DECLARE_RULE(decl_product_expression);
	MACH_GRAMMAR_RULE_DEF_PREFIX(decl_product_expression) = ((MACH_GRAMMAR_NAME(decl_grouped_expression) | MACH_GRAMMAR_NAME(decl_expression_term)) >> x3::char_("*/") >> (MACH_GRAMMAR_NAME(decl_product_expression) | MACH_GRAMMAR_NAME(decl_expression_term)));

	MACH_GRAMMAR_RULE_DEF_PREFIX(decl_sum_expression) = ((MACH_GRAMMAR_NAME(decl_product_expression) | MACH_GRAMMAR_NAME(decl_expression_term)) >> x3::char_("+-") >> (MACH_GRAMMAR_NAME(decl_sum_expression) | MACH_GRAMMAR_NAME(decl_expression_term)));

	MACH_GRAMMAR_DECLARE_RULE2(decl_sum_only_expression, decl_sum_expression);
	MACH_GRAMMAR_RULE_DEF_PREFIX(decl_sum_only_expression) = (MACH_GRAMMAR_NAME(decl_expression_term) >> x3::char_("+-") >> (MACH_GRAMMAR_NAME(decl_sum_only_expression) | MACH_GRAMMAR_NAME(decl_expression_term)));

	MACH_GRAMMAR_DECLARE_RULE(asm_offset_item);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_offset_item) = (MACH_GRAMMAR_SYMBOLS_NAME(asm_offset_op) > (MACH_GRAMMAR_NAME(asm_integral_value) | MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_identifier)));

	MACH_GRAMMAR_DECLARE_RULE(asm_offset);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_offset) = (x3::attr(false) >> (MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_identifier)) >> +MACH_GRAMMAR_NAME(asm_offset_item));

	MACH_GRAMMAR_DECLARE_RULE2(asm_offset_explicit, asm_offset);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_offset_explicit) = ((helper::keyword(x3::no_case["offset"]) >> x3::attr(true)) > (MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_identifier)) > +MACH_GRAMMAR_NAME(asm_offset_item));

	MACH_GRAMMAR_DECLARE_RULE(asm_memory);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_memory) = (-x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_type)] >> '[' > (MACH_GRAMMAR_NAME(asm_integral_value) | MACH_GRAMMAR_NAME(asm_identifier) | MACH_GRAMMAR_NAME(asm_offset)) > ']');

	MACH_GRAMMAR_DECLARE_RULE(asm_no_arg_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_no_arg_instruction) = x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_no_arg_instruction)];

	MACH_GRAMMAR_DECLARE_RULE2(asm_std_instruction, asm_pair_args_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_std_instruction) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_std_instruction)]) >
		(MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_memory)) >
		(MACH_GRAMMAR_NAME(asm_memory) |MACH_GRAMMAR_NAME(asm_offset_explicit) | MACH_GRAMMAR_NAME(asm_identifier) |
			MACH_GRAMMAR_NAME(asm_float_value) | MACH_GRAMMAR_NAME(asm_integral_value) | MACH_GRAMMAR_NAME(asm_char_value))
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_jmp_instruction, asm_single_arg_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_jmp_instruction) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_jmp_instruction)]) >
		(MACH_GRAMMAR_NAME(asm_offset_explicit) | MACH_GRAMMAR_NAME(asm_identifier) | MACH_GRAMMAR_NAME(asm_integral_value))
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_set_instruction, asm_single_arg_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_set_instruction) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_set_instruction)]) >
		MACH_GRAMMAR_NAME(asm_register)
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_stack_instruction, asm_single_arg_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_stack_instruction) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_stack_instruction)]) >
		(MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_memory))
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_unary_arith_instruction, asm_single_arg_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_unary_arith_instruction) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_unary_arith_instruction)]) >
		(MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_memory))
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_binary_arith_instruction, asm_pair_args_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_binary_arith_instruction) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_binary_arith_instruction)]) >
		(MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_memory)) >
		(MACH_GRAMMAR_NAME(asm_memory) | MACH_GRAMMAR_NAME(asm_identifier) | MACH_GRAMMAR_NAME(asm_float_value) | MACH_GRAMMAR_NAME(asm_integral_value))
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_binary_int_arith_instruction, asm_pair_args_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_binary_int_arith_instruction) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_binary_int_arith_instruction)]) >
		(MACH_GRAMMAR_NAME(asm_register) | MACH_GRAMMAR_NAME(asm_memory)) >
		(MACH_GRAMMAR_NAME(asm_memory) | MACH_GRAMMAR_NAME(asm_identifier) | MACH_GRAMMAR_NAME(asm_integral_value))
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_lea_instruction, asm_single_arg_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_lea_instruction) = (
		(helper::keyword(x3::no_case["lea"]) >> x3::attr(machine::op_code::lea)) >
		(MACH_GRAMMAR_NAME(asm_memory) | MACH_GRAMMAR_NAME(asm_offset_explicit) | MACH_GRAMMAR_NAME(asm_identifier) | MACH_GRAMMAR_NAME(asm_integral_value))
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_enter_instruction, asm_single_arg_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_enter_instruction) = (
		(helper::keyword(x3::no_case["enter"]) >> x3::attr(machine::op_code::enter)) >
		MACH_GRAMMAR_NAME(asm_integral_value)
	);
	
	MACH_GRAMMAR_DECLARE_RULE2(asm_cnvt_instruction, asm_pair_args_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_cnvt_instruction) = (
		(helper::keyword(x3::no_case["cnvt"]) >> x3::attr(machine::op_code::cnvt)) >
		MACH_GRAMMAR_NAME(asm_register) >
		MACH_GRAMMAR_NAME(asm_register)
	);
	
	MACH_GRAMMAR_DECLARE_RULE(asm_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_instruction) = (
		MACH_GRAMMAR_NAME(asm_no_arg_instruction) |
		MACH_GRAMMAR_NAME(asm_std_instruction) |
		MACH_GRAMMAR_NAME(asm_jmp_instruction) |
		MACH_GRAMMAR_NAME(asm_set_instruction) |
		MACH_GRAMMAR_NAME(asm_stack_instruction) |
		MACH_GRAMMAR_NAME(asm_binary_arith_instruction) |
		MACH_GRAMMAR_NAME(asm_binary_int_arith_instruction) |
		MACH_GRAMMAR_NAME(asm_lea_instruction) |
		MACH_GRAMMAR_NAME(asm_enter_instruction) |
		MACH_GRAMMAR_NAME(asm_cnvt_instruction)
	);
	
	MACH_GRAMMAR_DECLARE_RULE(asm_times_instruction);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_times_instruction) = (helper::keyword(x3::no_case["times"]) > x3::uint64 > MACH_GRAMMAR_NAME(asm_instruction));
	
	MACH_GRAMMAR_DECLARE_RULE(asm_db);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_db) = (
		helper::keyword(x3::no_case[MACH_GRAMMAR_SYMBOLS_NAME(asm_decl_type)]) >
		+(
			MACH_GRAMMAR_NAME(asm_uninitialized_value) |
			MACH_GRAMMAR_NAME(decl_sum_expression) |
			MACH_GRAMMAR_NAME(decl_product_expression) |
			MACH_GRAMMAR_NAME(decl_grouped_expression) |
			MACH_GRAMMAR_NAME(asm_float_value) |
			MACH_GRAMMAR_NAME(asm_integral_value) |
			MACH_GRAMMAR_NAME(asm_char_value) |
			MACH_GRAMMAR_NAME(asm_string_value) |
			MACH_GRAMMAR_NAME(asm_identifier)
		)
	);

	MACH_GRAMMAR_DECLARE_RULE(asm_dz);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_dz) = (helper::keyword(x3::no_case[".zero"]) > x3::uint64);

	MACH_GRAMMAR_DECLARE_RULE(asm_ds);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_ds) = (helper::keyword(x3::no_case[".string"]) > MACH_GRAMMAR_NAME(asm_string_value));

	MACH_GRAMMAR_DECLARE_RULE(asm_decl);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_decl) = (MACH_GRAMMAR_NAME(asm_identifier) >> (MACH_GRAMMAR_NAME(asm_db) | MACH_GRAMMAR_NAME(asm_dz) | MACH_GRAMMAR_NAME(asm_ds)));

	MACH_GRAMMAR_DECLARE_RULE(asm_equ);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_equ) = (MACH_GRAMMAR_NAME(asm_identifier) >> helper::keyword(x3::no_case["equ"]) > (MACH_GRAMMAR_NAME(decl_sum_only_expression) | MACH_GRAMMAR_NAME(asm_integral_value) | MACH_GRAMMAR_NAME(asm_identifier)));

	MACH_GRAMMAR_DECLARE_RULE(asm_statement);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_statement) = ((
		MACH_GRAMMAR_NAME(asm_section) |
		MACH_GRAMMAR_NAME(asm_stack) |
		MACH_GRAMMAR_NAME(asm_global) |
		MACH_GRAMMAR_NAME(asm_label) |
		MACH_GRAMMAR_NAME(asm_instruction) |
		MACH_GRAMMAR_NAME(asm_times_instruction) |
		MACH_GRAMMAR_NAME(asm_db) |
		MACH_GRAMMAR_NAME(asm_dz) |
		MACH_GRAMMAR_NAME(asm_ds) |
		MACH_GRAMMAR_NAME(asm_decl) |
		MACH_GRAMMAR_NAME(asm_equ)
	) > (x3::eol | x3::eoi));

	MACH_GRAMMAR_DECLARE_RULE_WITH_ERROR_HANDLER(asm_program, asm_program);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_program) = (*MACH_GRAMMAR_NAME(asm_statement) > !x3::char_);

	MACH_GRAMMAR_DECLARE_RULE0(asm_skip);
	MACH_GRAMMAR_RULE_DEF_PREFIX(asm_skip) = x3::omit[((x3::space - x3::eol) | (';' >> *(x3::char_ - x3::eol)))];

	BOOST_SPIRIT_DEFINE(
		MACH_GRAMMAR_NAME(asm_uninitialized_value),
		MACH_GRAMMAR_NAME(asm_integral_value),
		MACH_GRAMMAR_NAME(asm_float_value),
		MACH_GRAMMAR_NAME(asm_char_value),
		MACH_GRAMMAR_NAME(asm_expected_char_value),
		MACH_GRAMMAR_NAME(asm_string_value),
		MACH_GRAMMAR_NAME(asm_register),
		MACH_GRAMMAR_NAME(asm_not_identifier),
		MACH_GRAMMAR_NAME(asm_identifier_part),
		MACH_GRAMMAR_NAME(asm_identifier),
		MACH_GRAMMAR_NAME(asm_section),
		MACH_GRAMMAR_NAME(asm_label),
		MACH_GRAMMAR_NAME(asm_stack),
		MACH_GRAMMAR_NAME(asm_global),
		MACH_GRAMMAR_NAME(decl_expression_term),
		MACH_GRAMMAR_NAME(decl_grouped_expression),
		MACH_GRAMMAR_NAME(decl_sum_only_expression),
		MACH_GRAMMAR_NAME(decl_sum_expression),
		MACH_GRAMMAR_NAME(decl_product_expression),
		MACH_GRAMMAR_NAME(asm_offset_item),
		MACH_GRAMMAR_NAME(asm_offset),
		MACH_GRAMMAR_NAME(asm_offset_explicit),
		MACH_GRAMMAR_NAME(asm_memory),
		MACH_GRAMMAR_NAME(asm_no_arg_instruction),
		MACH_GRAMMAR_NAME(asm_std_instruction),
		MACH_GRAMMAR_NAME(asm_jmp_instruction),
		MACH_GRAMMAR_NAME(asm_set_instruction),
		MACH_GRAMMAR_NAME(asm_stack_instruction),
		MACH_GRAMMAR_NAME(asm_unary_arith_instruction),
		MACH_GRAMMAR_NAME(asm_binary_arith_instruction),
		MACH_GRAMMAR_NAME(asm_binary_int_arith_instruction),
		MACH_GRAMMAR_NAME(asm_lea_instruction),
		MACH_GRAMMAR_NAME(asm_enter_instruction),
		MACH_GRAMMAR_NAME(asm_cnvt_instruction),
		MACH_GRAMMAR_NAME(asm_instruction),
		MACH_GRAMMAR_NAME(asm_times_instruction),
		MACH_GRAMMAR_NAME(asm_db),
		MACH_GRAMMAR_NAME(asm_dz),
		MACH_GRAMMAR_NAME(asm_ds),
		MACH_GRAMMAR_NAME(asm_decl),
		MACH_GRAMMAR_NAME(asm_equ),
		MACH_GRAMMAR_NAME(asm_statement),
		MACH_GRAMMAR_NAME(asm_program),
		MACH_GRAMMAR_NAME(asm_skip)
	)
}
