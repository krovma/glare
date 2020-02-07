#pragma once
#include "glare/core/common.h"
#include <vector>
#include <string>

//////////////////////////////////////////////////////////////////////////
namespace glare
{
string format(const char* fmt, ...);
size_t load_file_to_string (string& out_string, const char* path);
std::vector<string> split(const char* cstr, char delimiter=' ', bool collapse_multi_delimiters=true, bool return_delimiters=false);
};