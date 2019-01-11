#pragma once

#include "io_reader.h"

namespace mach::io{
	class binary_reader : public reader{
	public:
		virtual ~binary_reader() = default;
	};
}
