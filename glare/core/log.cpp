#include "glare/core/log.h"
#include "glare/core/assert.h"
#include <cstdarg>
#include <iostream>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
namespace glare
{
void debug_log(const char* fmt, ...)
{
#if defined(_WIN32)
	constexpr const size_t BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	va_list v;
	va_start(v, fmt);
	vsnprintf_s(buffer, BUFFER_SIZE, _TRUNCATE, fmt, v);
	va_end(v);
	buffer[BUFFER_SIZE - 1] = '\0';
	if (is_debugger_available()) {
		OutputDebugStringA(buffer);
	}
#endif
	std::cout << buffer;
}
};