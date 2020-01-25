#include "glare/core/string_utilities.h"
#include <stdarg.h>

namespace glare {
////////////////////////////////
string format(const char* fmt, ...)
{
	constexpr size_t BUFFER_SIZE = 2048;
	char buffer[BUFFER_SIZE];
	va_list va;
	va_start(va, fmt);
	vsnprintf_s(buffer, BUFFER_SIZE, fmt, va);
	va_end(va);
	buffer[BUFFER_SIZE - 1] = 0;
	return string(buffer);
}

};