#pragma once

namespace mach::machine{
	enum class op_code : unsigned int{
		nop,
		movbri,
		movbrr,
		movbrm,
		movbmi,
		movbmr,
		movwri,
		movwrr,
		movwrm,
		movwmi,
		movwmr,
		movdri,
		movdrr,
		movdrm,
		movdmi,
		movdmr,
		movqri,
		movqrr,
		movqrm,
		movqmi,
		movqmr,
		movfri,
		movfrr,
		movfrm,
		movfmi,
		movfmr,
	};
}
