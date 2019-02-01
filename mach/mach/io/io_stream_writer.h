#pragma once

#include "io_writer.h"

namespace mach::io{
	class stream_writer : public writer{
	public:
		virtual ~stream_writer() = default;

		virtual void write(bool data) = 0;

		virtual void write(char data) = 0;

		virtual void write(__int16 data) = 0;

		virtual void write(unsigned __int16 data) = 0;

		virtual void write(__int32 data) = 0;

		virtual void write(unsigned __int32 data) = 0;

		virtual void write(__int64 data) = 0;

		virtual void write(unsigned __int64 data) = 0;

		virtual void write(float data) = 0;

		virtual void write(double data) = 0;

		virtual void write(long double data) = 0;

		virtual void write(const char *buffer, std::size_t size) = 0;

		virtual void write(const char *buffer) = 0;
	};
}
