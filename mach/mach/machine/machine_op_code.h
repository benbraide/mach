#pragma once

namespace mach::machine{
	enum class op_operand_type : unsigned __int8{
		nil,
		reg,
		mem,
		val,
	};

	enum class op_operand_size : unsigned __int8{
		nil,
		byte	= sizeof(unsigned __int8),
		word	= sizeof(unsigned __int16),
		dword	= sizeof(unsigned __int32),
		qword	= sizeof(unsigned __int64),
		dfloat	= sizeof(float),
		qfloat	= sizeof(long double),
	};

	enum class op_offset_operator : unsigned __int8{
		nil,
		plus,
		minus,
	};

	enum class op_code : unsigned __int8{
		nop,
		mov,
	};
}
