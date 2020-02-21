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

template<typename T>
string repr(const T& v, std::enable_if_t<std::is_class_v<T>>* _ = nullptr)
{
	return v.repr();
}

template<typename T>
string repr(const T& v, std::enable_if_t<std::is_integral_v<T>>* _ = nullptr )
{
	return format("%i", v);
}

template<typename T>
string repr(const T& v, std::enable_if_t<std::is_floating_point_v<T>>* _ = nullptr )
{
	return format("%g", v);
}

template<typename T>
string repr(const T& v, std::enable_if_t<std::is_pointer_v<T>>* _ = nullptr)
{
	return format("%p", v);
}

inline string repr(const string& v)
{
	return v;
}

inline string repr(const bool& v)
{
	return v ? "True" : "False";
}

};