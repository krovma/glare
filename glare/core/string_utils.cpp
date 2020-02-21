#include "glare/core/string_utils.h"
#include "glare/core/assert.h"
#include <fstream>
#include <sstream>
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

size_t load_file_to_string(string& out_string, const char* path)
{
	std::ifstream fin;
	std::stringstream stream;
	try {
		fin.open(path);
		stream << fin.rdbuf();
		fin.close();
	} catch(std::ifstream::failure& e) {
		FATAL(format("Failed when reading %s\n(%s)", path, e.what()));
	}
	out_string = stream.str();
	return out_string.length();
}

///////////////////////////////
std::vector<string> split(
	const char* cstr, char delimiter, bool collapse_multi_delimiters, bool return_delimiters)
{
	std::vector<string> result {""};
	size_t result_idx = 0;
	for (size_t i = 0; cstr[i] != '\0'; ++i) {
		if (cstr[i] != delimiter) {
			result[result_idx].append(1, cstr[i]);
		} else {
			if(result[result_idx].empty()) {
				if (collapse_multi_delimiters && !return_delimiters) {
					continue;
				}
			}
			if(return_delimiters) {
				result[result_idx].append(1, cstr[i]);
			}
			result.emplace_back("");
			++result_idx;
		}
	}
	if (result[result.size() - 1].empty()) {
		result.pop_back();
	}
	return result;
}
};