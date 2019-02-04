#include <iostream>

#include "machine/machine_memory.h"
#include "machine/machine_register_table.h"

#include "parsing/asm_grammar.h"

int main(){
	std::string data = "mov eax raxa. ball";
	MACH_AST_QNAME(asm_program) ast;

	auto begin = data.begin();
	auto end = data.end();

	boost::spirit::x3::error_handler<std::string::iterator> error_handler(begin, end, std::cerr);
	mach::asm_code::translation_state ts;

	auto const parser_with_ts = boost::spirit::x3::with<MACH_GRAMMAR_TAG_QNAME(asm_translation_state)>(std::ref(ts))[MACH_GRAMMAR_QNAME(asm_program)];
	auto const parser = boost::spirit::x3::with<boost::spirit::x3::error_handler_tag>(std::ref(error_handler))[parser_with_ts];

	auto result = boost::spirit::x3::phrase_parse(
		begin,
		end,
		parser,
		MACH_GRAMMAR_QNAME(asm_skip),
		ast
	);

	return 0;
}
