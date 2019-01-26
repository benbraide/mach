#pragma once

#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/adapted/mpl.hpp>

#include <boost/spirit/home/x3/support/ast/position_tagged.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/spirit/home/x3.hpp>

#define MACH_AST_JOIN_(x, y) x ## y
#define MACH_AST_JOIN(x, y) MACH_AST_JOIN_(x, y)

#define MACH_AST_NAME(n) MACH_AST_JOIN(n, _ast)
#define MACH_AST_QNAME(n) mach::parsing::MACH_AST_NAME(n)

#define MACH_AST_SINGLE_FIELD_TYPE value_type
#define MACH_AST_SINGLE_FIELD_NAME value

#define MACH_AST_MULTIPLE_FIRST_FIELD_TYPE first_value_type
#define MACH_AST_MULTIPLE_FIRST_FIELD_NAME first_value

#define MACH_AST_MULTIPLE_SECOND_FIELD_TYPE second_value_type
#define MACH_AST_MULTIPLE_SECOND_FIELD_NAME second_value

#define MACH_AST_MULTIPLE_THIRD_FIELD_TYPE third_value_type
#define MACH_AST_MULTIPLE_THIRD_FIELD_NAME third_value

#define MACH_AST_TEMPLATE_TYPE(n, ...) n<__VA_ARGS__>
#define MACH_AST_VARIANT(...) MACH_AST_TEMPLATE_TYPE(boost::variant, __VA_ARGS__)

#define MACH_AST_WPOS boost::spirit::x3::position_tagged

#define MACH_AST_DECLARE_NAME(n) struct MACH_AST_NAME(n);

#define MACH_AST_DECLARE_SINGLE(n, t)																\
struct MACH_AST_NAME(n){																			\
	using MACH_AST_SINGLE_FIELD_TYPE = t;															\
	MACH_AST_SINGLE_FIELD_TYPE MACH_AST_SINGLE_FIELD_NAME;											\
};

#define MACH_AST_DECLARE_SINGLE_WPOS(n, t)															\
struct MACH_AST_NAME(n) : MACH_AST_WPOS{															\
	using MACH_AST_SINGLE_FIELD_TYPE = t;															\
	MACH_AST_SINGLE_FIELD_TYPE MACH_AST_SINGLE_FIELD_NAME;											\
};

#define MACH_AST_DECLARE_SINGLE_VARIANT(n, ...)														\
struct MACH_AST_NAME(n){																			\
	using MACH_AST_SINGLE_FIELD_TYPE = MACH_AST_VARIANT(__VA_ARGS__);								\
	MACH_AST_SINGLE_FIELD_TYPE MACH_AST_SINGLE_FIELD_NAME;											\
};

#define MACH_AST_DECLARE_SINGLE_VARIANT_WPOS(n, ...)												\
struct MACH_AST_NAME(n) : MACH_AST_WPOS{															\
	using MACH_AST_SINGLE_FIELD_TYPE = MACH_AST_VARIANT(__VA_ARGS__);								\
	MACH_AST_SINGLE_FIELD_TYPE MACH_AST_SINGLE_FIELD_NAME;											\
};

#define MACH_AST_DECLARE_PAIR(n, t1, t2)															\
struct MACH_AST_NAME(n){																			\
	using MACH_AST_MULTIPLE_FIRST_FIELD_TYPE = t1;													\
	using MACH_AST_MULTIPLE_SECOND_FIELD_TYPE = t2;													\
	MACH_AST_MULTIPLE_FIRST_FIELD_TYPE MACH_AST_MULTIPLE_FIRST_FIELD_NAME;							\
	MACH_AST_MULTIPLE_SECOND_FIELD_TYPE MACH_AST_MULTIPLE_SECOND_FIELD_NAME;						\
};

#define MACH_AST_DECLARE_PAIR_WPOS(n, t1, t2)														\
struct MACH_AST_NAME(n) : MACH_AST_WPOS{															\
	using MACH_AST_MULTIPLE_FIRST_FIELD_TYPE = t1;													\
	using MACH_AST_MULTIPLE_SECOND_FIELD_TYPE = t2;													\
	MACH_AST_MULTIPLE_FIRST_FIELD_TYPE MACH_AST_MULTIPLE_FIRST_FIELD_NAME;							\
	MACH_AST_MULTIPLE_SECOND_FIELD_TYPE MACH_AST_MULTIPLE_SECOND_FIELD_NAME;						\
};

#define MACH_AST_DECLARE_TRIO(n, t1, t2, t3)														\
struct MACH_AST_NAME(n){																			\
	using MACH_AST_MULTIPLE_FIRST_FIELD_TYPE = t1;													\
	using MACH_AST_MULTIPLE_SECOND_FIELD_TYPE = t2;													\
	using MACH_AST_MULTIPLE_THIRD_FIELD_TYPE = t3;													\
	MACH_AST_MULTIPLE_FIRST_FIELD_TYPE MACH_AST_MULTIPLE_FIRST_FIELD_NAME;							\
	MACH_AST_MULTIPLE_SECOND_FIELD_TYPE MACH_AST_MULTIPLE_SECOND_FIELD_NAME;						\
	MACH_AST_MULTIPLE_THIRD_FIELD_TYPE MACH_AST_MULTIPLE_THIRD_FIELD_NAME;							\
};

#define MACH_AST_DECLARE_TRIO_WPOS(n, t1, t2, t3)													\
struct MACH_AST_NAME(n) : MACH_AST_WPOS{															\
	using MACH_AST_MULTIPLE_FIRST_FIELD_TYPE = t1;													\
	using MACH_AST_MULTIPLE_SECOND_FIELD_TYPE = t2;													\
	using MACH_AST_MULTIPLE_THIRD_FIELD_TYPE = t3;													\
	MACH_AST_MULTIPLE_FIRST_FIELD_TYPE MACH_AST_MULTIPLE_FIRST_FIELD_NAME;							\
	MACH_AST_MULTIPLE_SECOND_FIELD_TYPE MACH_AST_MULTIPLE_SECOND_FIELD_NAME;						\
	MACH_AST_MULTIPLE_THIRD_FIELD_TYPE MACH_AST_MULTIPLE_THIRD_FIELD_NAME;							\
};

#define MACH_AST_ADAPT_SINGLE(n)																	\
BOOST_FUSION_ADAPT_STRUCT(																			\
	MACH_AST_QNAME(n),																				\
	(MACH_AST_QNAME(n)::MACH_AST_SINGLE_FIELD_TYPE, MACH_AST_SINGLE_FIELD_NAME)						\
)

#define MACH_AST_ADAPT_PAIR(n)																		\
BOOST_FUSION_ADAPT_STRUCT(																			\
	MACH_AST_QNAME(n),																				\
	(MACH_AST_QNAME(n)::MACH_AST_MULTIPLE_FIRST_FIELD_TYPE, MACH_AST_MULTIPLE_FIRST_FIELD_NAME)		\
	(MACH_AST_QNAME(n)::MACH_AST_MULTIPLE_SECOND_FIELD_TYPE, MACH_AST_MULTIPLE_SECOND_FIELD_NAME)	\
)

#define MACH_AST_ADAPT_TRIO(n)																		\
BOOST_FUSION_ADAPT_STRUCT(																			\
	MACH_AST_QNAME(n),																				\
	(MACH_AST_QNAME(n)::MACH_AST_MULTIPLE_FIRST_FIELD_TYPE, MACH_AST_MULTIPLE_FIRST_FIELD_NAME)		\
	(MACH_AST_QNAME(n)::MACH_AST_MULTIPLE_SECOND_FIELD_TYPE, MACH_AST_MULTIPLE_SECOND_FIELD_NAME)	\
	(MACH_AST_QNAME(n)::MACH_AST_MULTIPLE_THIRD_FIELD_TYPE, MACH_AST_MULTIPLE_THIRD_FIELD_NAME)		\
)

namespace mach::parsing{
	MACH_AST_DECLARE_SINGLE(mach_identifier, std::string)
}

MACH_AST_ADAPT_SINGLE(mach_identifier)
