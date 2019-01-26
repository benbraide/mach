#pragma once

namespace mach::machine{
	enum class op_operand_type : unsigned __int8{
		nil,
		reg,
		mem,
		mem_with_offset,
		val,
	};

	enum class op_operand_size : unsigned __int8{
		nil,
		byte	= static_cast<unsigned __int8>(sizeof(unsigned __int8)),
		word	= static_cast<unsigned __int8>(sizeof(unsigned __int16)),
		dword	= static_cast<unsigned __int8>(sizeof(unsigned __int32)),
		qword	= static_cast<unsigned __int8>(sizeof(unsigned __int64)),
		dfloat	= static_cast<unsigned __int8>(sizeof(float)),
		qfloat	= static_cast<unsigned __int8>(sizeof(long double)),
		error	= static_cast<unsigned __int8>(-1),
	};

	enum class op_offset_operator : unsigned __int8{
		nil,
		plus,
		minus,
	};

	enum class op_code : unsigned __int8{
		nop,
		push,
		pop,
		mov,
		lea,
		syscall,
		call,
		enter,
		leave,
		ret,
		jmp,
		jz,
		jnz,
		je,
		jne,
		jl,
		jnl,
		jle,
		jnle,
		jg,
		jng,
		jge,
		jnge,
		setz,
		setnz,
		sete,
		setne,
		setl,
		setnl,
		setle,
		setnle,
		setg,
		setng,
		setge,
		setnge,
		loop,
		inc,
		dec,
		add,
		sub,
		mult,
		div,
		mod,
		and_,
		xor_,
		or_,
		sal,
		sar,
		test,
		not_,
		cmp,
	};
}
