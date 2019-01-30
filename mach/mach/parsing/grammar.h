#pragma once

#include <boost/spirit/home/x3/support/utility/annotate_on_success.hpp>

#include "ast.h"

#define MACH_GRAMMAR_JOIN_(x, y) x ## y
#define MACH_GRAMMAR_JOIN(x, y) MACH_GRAMMAR_JOIN_(x, y)

#define MACH_GRAMMAR_TO_STRING_(x) #x
#define MACH_GRAMMAR_TO_STRING(x) MACH_GRAMMAR_TO_STRING_(x)

#define MACH_GRAMMAR_NAME(n) MACH_GRAMMAR_JOIN(n, _grammar)
#define MACH_GRAMMAR_QNAME(n) mach::parsing::MACH_GRAMMAR_NAME(n)

#define MACH_GRAMMAR_CLASS_NAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_NAME(n), _class)
#define MACH_GRAMMAR_CLASS_QNAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_QNAME(n), _class)

#define MACH_GRAMMAR_NAME_STR(n) MACH_GRAMMAR_TO_STRING(MACH_GRAMMAR_NAME(n))

#define MACH_GRAMMAR_SYMBOLS_CLASS_NAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_NAME(n), _symbols)
#define MACH_GRAMMAR_SYMBOLS_CLASS_QNAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_QNAME(n), _symbols)

#define MACH_GRAMMAR_SYMBOLS_NAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_SYMBOLS_CLASS_NAME(n), _instance)
#define MACH_GRAMMAR_SYMBOLS_QNAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_SYMBOLS_CLASS_QNAME(n), _instance)

#define MACH_GRAMMAR_TAG_NAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_NAME(n), _tag)
#define MACH_GRAMMAR_TAG_QNAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_QNAME(n), _tag)

#define MACH_GRAMMAR_ACTION_NAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_NAME(n), _action)
#define MACH_GRAMMAR_ACTION_QNAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_QNAME(n), _action)

#define MACH_GRAMMAR_ACTION_INSTANCE(n) MACH_GRAMMAR_ACTION_NAME(n)()
#define MACH_GRAMMAR_ACTION_QINSTANCE(n) MACH_GRAMMAR_ACTION_QNAME(n)()

#define MACH_GRAMMAR_ANNOTATOR_NAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_NAME(n), _annotator)
#define MACH_GRAMMAR_ANNOTATOR_QNAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_QNAME(n), _annotator)

#define MACH_GRAMMAR_ERROR_HANDLER_NAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_NAME(n), _error_handler)
#define MACH_GRAMMAR_ERROR_HANDLER_QNAME(n) MACH_GRAMMAR_JOIN(MACH_GRAMMAR_QNAME(n), _error_handler)

#define MACH_GRAMMAR_DECLARE_RULE(n)																			\
struct MACH_GRAMMAR_CLASS_NAME(n) : boost::spirit::x3::annotate_on_success{};									\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(n)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE0(n)																			\
boost::spirit::x3::rule<struct MACH_GRAMMAR_CLASS_NAME(n)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE_WITH_ERROR_HANDLER(n, e)														\
struct MACH_GRAMMAR_CLASS_NAME(n) : MACH_GRAMMAR_ERROR_HANDLER_NAME(e), boost::spirit::x3::annotate_on_success{};\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(n)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE2(n, ast)																		\
struct MACH_GRAMMAR_CLASS_NAME(n) : boost::spirit::x3::annotate_on_success{};									\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(ast)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE2_WITH_ERROR_HANDLER(n, ast, e)												\
struct MACH_GRAMMAR_CLASS_NAME(n) : MACH_GRAMMAR_ERROR_HANDLER_NAME(e), boost::spirit::x3::annotate_on_success{};\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(ast)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE3(n, a)																		\
struct MACH_GRAMMAR_CLASS_NAME(n) : MACH_GRAMMAR_ANNOTATOR_NAME(a){};											\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(n)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE3_WITH_ERROR_HANDLER(n, a, e)													\
struct MACH_GRAMMAR_CLASS_NAME(n) : MACH_GRAMMAR_ERROR_HANDLER_NAME(e), MACH_GRAMMAR_ANNOTATOR_NAME(a){};		\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(n)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE4(n, ast, a)																	\
struct MACH_GRAMMAR_CLASS_NAME(n) : MACH_GRAMMAR_ANNOTATOR_NAME(a){};											\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(ast)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_DECLARE_RULE4_WITH_ERROR_HANDLER(n, ast, a, e)												\
struct MACH_GRAMMAR_CLASS_NAME(n) : MACH_GRAMMAR_ERROR_HANDLER_NAME(e), MACH_GRAMMAR_ANNOTATOR_NAME(a){};		\
boost::spirit::x3::rule<MACH_GRAMMAR_CLASS_NAME(n), MACH_AST_QNAME(ast)> const MACH_GRAMMAR_NAME(n) = MACH_GRAMMAR_NAME_STR(n);

#define MACH_GRAMMAR_RULE_DEF_PREFIX(n) auto const MACH_GRAMMAR_JOIN(MACH_GRAMMAR_NAME(n), _def)

#define MACH_GRAMMAR_SYMBOLS_BEGIN(n, t)																		\
struct MACH_GRAMMAR_SYMBOLS_CLASS_NAME(n) : boost::spirit::x3::symbols<t>{										\
	MACH_GRAMMAR_SYMBOLS_CLASS_NAME(n)(){																		\
		add

#define MACH_GRAMMAR_SYMBOLS_END(n)																				\
		;																										\
	}																											\
} MACH_GRAMMAR_SYMBOLS_NAME(n);

#define MACH_GRAMMAR_ACTION_CONTEXT_TYPE mach_context_type
#define MACH_GRAMMAR_ACTION_CONTEXT_NAME mach_context

#define MACH_GRAMMAR_ACTION_BEGIN(n)																			\
struct MACH_GRAMMAR_ACTION_NAME(n){																				\
	template <typename MACH_GRAMMAR_ACTION_CONTEXT_TYPE>														\
	void operator()(MACH_GRAMMAR_ACTION_CONTEXT_TYPE const &MACH_GRAMMAR_ACTION_CONTEXT_NAME) const{			\

#define MACH_GRAMMAR_ACTION_END																					\
	}																											\
};

#define MACH_GRAMMAR_ACTION_ATTR boost::spirit::x3::_attr(MACH_GRAMMAR_ACTION_CONTEXT_NAME)
#define MACH_GRAMMAR_ACTION_VAL boost::spirit::x3::_val(MACH_GRAMMAR_ACTION_CONTEXT_NAME)

#define MACH_GRAMMAR_ACTION_ATTR_TO_VAL MACH_GRAMMAR_ACTION_VAL = MACH_GRAMMAR_ACTION_ATTR
#define MACH_GRAMMAR_ACTION_FAIL boost::spirit::x3::_pass(MACH_GRAMMAR_ACTION_CONTEXT_NAME) = false

#define MACH_GRAMMAR_ACTION_GET_TAGGED(n) boost::spirit::x3::get<MACH_GRAMMAR_TAG_NAME(n)>(MACH_GRAMMAR_ACTION_CONTEXT_NAME).get()

#define MACH_GRAMMAR_ANNOTATOR_BASE_TYPE mach_base_type
#define MACH_GRAMMAR_ANNOTATOR_AST_TYPE mach_ast_type
#define MACH_GRAMMAR_ANNOTATOR_ITERATOR_TYPE mach_iterator_type
#define MACH_GRAMMAR_ANNOTATOR_CONTEXT_TYPE mach_context_type

#define MACH_GRAMMAR_ANNOTATOR_AST_NAME mach_ast
#define MACH_GRAMMAR_ANNOTATOR_FIRST_ITERATOR_NAME mach_first
#define MACH_GRAMMAR_ANNOTATOR_LAST_ITERATOR_NAME mach_last
#define MACH_GRAMMAR_ANNOTATOR_CONTEXT_NAME mach_context

#define MACH_GRAMMAR_ANNOTATOR_BEGIN(n)																			\
struct MACH_GRAMMAR_ANNOTATOR_NAME(n) : boost::spirit::x3::annotate_on_success{									\
	using MACH_GRAMMAR_ANNOTATOR_BASE_TYPE = boost::spirit::x3::annotate_on_success;							\
	template <																									\
		typename MACH_GRAMMAR_ANNOTATOR_AST_TYPE,																\
		typename MACH_GRAMMAR_ANNOTATOR_ITERATOR_TYPE,															\
		typename MACH_GRAMMAR_ANNOTATOR_CONTEXT_TYPE															\
	>																											\
	void on_success(																							\
		const MACH_GRAMMAR_ANNOTATOR_ITERATOR_TYPE &MACH_GRAMMAR_ANNOTATOR_FIRST_ITERATOR_NAME,					\
		const MACH_GRAMMAR_ANNOTATOR_ITERATOR_TYPE &MACH_GRAMMAR_ANNOTATOR_LAST_ITERATOR_NAME,					\
		MACH_GRAMMAR_ANNOTATOR_AST_TYPE &MACH_GRAMMAR_ANNOTATOR_AST_NAME,										\
		const MACH_GRAMMAR_ANNOTATOR_CONTEXT_TYPE &MACH_GRAMMAR_ANNOTATOR_CONTEXT_NAME							\
	){

#define MACH_GRAMMAR_ANNOTATOR_END																				\
	}																											\
};

#define MACH_GRAMMAR_ANNOTATOR_CALL_BASE																		\
MACH_GRAMMAR_ANNOTATOR_BASE_TYPE::on_success(																	\
	MACH_GRAMMAR_ANNOTATOR_FIRST_ITERATOR_NAME,																	\
	MACH_GRAMMAR_ANNOTATOR_LAST_ITERATOR_NAME,																	\
	MACH_GRAMMAR_ANNOTATOR_AST_NAME,																			\
	MACH_GRAMMAR_ANNOTATOR_CONTEXT_NAME																			\
);

#define MACH_GRAMMAR_ANNOTATOR_FAIL boost::spirit::x3::_pass(MACH_GRAMMAR_ANNOTATOR_CONTEXT_NAME) = false

#define MACH_GRAMMAR_ERROR_HANDLER_ERROR_TYPE mach_error_type
#define MACH_GRAMMAR_ERROR_HANDLER_ITERATOR_TYPE mach_iterator_type
#define MACH_GRAMMAR_ERROR_HANDLER_CONTEXT_TYPE mach_context_type

#define MACH_GRAMMAR_ERROR_HANDLER_ERROR_NAME mach_e
#define MACH_GRAMMAR_ERROR_HANDLER_FIRST_ITERATOR_NAME mach_first
#define MACH_GRAMMAR_ERROR_HANDLER_LAST_ITERATOR_NAME mach_last
#define MACH_GRAMMAR_ERROR_HANDLER_CONTEXT_NAME mach_context

#define MACH_GRAMMAR_ERROR_HANDLER_BEGIN(n)																		\
struct MACH_GRAMMAR_ERROR_HANDLER_NAME(n){																		\
	template <																									\
		typename MACH_GRAMMAR_ERROR_HANDLER_ITERATOR_TYPE,														\
		typename MACH_GRAMMAR_ERROR_HANDLER_ERROR_TYPE,															\
		typename MACH_GRAMMAR_ERROR_HANDLER_CONTEXT_TYPE														\
	>																											\
	boost::spirit::x3::error_handler_result on_error(															\
		const MACH_GRAMMAR_ERROR_HANDLER_ITERATOR_TYPE &MACH_GRAMMAR_ERROR_HANDLER_FIRST_ITERATOR_NAME,			\
		const MACH_GRAMMAR_ERROR_HANDLER_ITERATOR_TYPE &MACH_GRAMMAR_ERROR_HANDLER_LAST_ITERATOR_NAME,			\
		const MACH_GRAMMAR_ERROR_HANDLER_ERROR_TYPE &MACH_GRAMMAR_ERROR_HANDLER_ERROR_NAME,						\
		const MACH_GRAMMAR_ERROR_HANDLER_CONTEXT_TYPE &MACH_GRAMMAR_ERROR_HANDLER_CONTEXT_NAME					\
	){

#define MACH_GRAMMAR_ERROR_HANDLER_END																			\
	}																											\
};

#define MACH_GRAMMAR_ERROR_HANDLER_HANDLE_ERROR																	\
auto &error_handler = boost::spirit::x3::get<boost::spirit::x3::error_handler_tag>(MACH_GRAMMAR_ERROR_HANDLER_CONTEXT_NAME).get();\
std::string message = "Error! Expecting: " + MACH_GRAMMAR_ERROR_HANDLER_ERROR_NAME.which() + " here:";			\
error_handler(MACH_GRAMMAR_ERROR_HANDLER_ERROR_NAME.where(), message);											\
return boost::spirit::x3::error_handler_result::fail;

namespace mach::parsing{
	namespace x3 = boost::spirit::x3;

	template <typename value_type>
	struct mach_real_policies : x3::real_policies<value_type>{
		static bool const expect_dot = true;
		static bool const allow_leading_dot = true;
		static bool const allow_trailing_dot = true;
	};

	using mach_float_parser = x3::real_parser<float, mach_real_policies<float>>;
	using mach_double_parser = x3::real_parser<double, mach_real_policies<double>>;
	using mach_long_double_parser = x3::real_parser<long double, mach_real_policies<long double>>;

	mach_float_parser const mach_float = {};
	mach_double_parser const mach_double = {};
	mach_long_double_parser const mach_long_double = {};

	template <typename target_type>
	using mach_hex_parser = x3::uint_parser<target_type, 16>;

	template <typename target_type>
	using mach_dec_parser = x3::uint_parser<target_type, 10>;

	template <typename target_type>
	using mach_oct_parser = x3::uint_parser<target_type, 8>;

	template <typename target_type>
	using mach_bin_parser = x3::uint_parser<target_type, 2>;

	MACH_GRAMMAR_DECLARE_RULE(mach_identifier);
	MACH_GRAMMAR_RULE_DEF_PREFIX(mach_identifier) = (&x3::char_("$_A-Za-z") >> x3::lexeme[+x3::char_("$_A-Za-z0-9")]);

	BOOST_SPIRIT_DEFINE(MACH_GRAMMAR_NAME(mach_identifier));

	class helper{
	public:
		template <typename rule_type>
		static auto keyword(rule_type &&rule){
			return x3::lexeme[rule >> &(x3::space | x3::eol | x3::eoi)];
		};

		static auto keyword(const char *value){
			return keyword(x3::lit(value));
		}
	};

	struct MACH_GRAMMAR_TAG_NAME(asm_error_handler);
}
