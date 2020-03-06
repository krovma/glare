#pragma once
#include "glare/core/common.h"
#include <vector>
#include <string>
#include <cstdlib>
#include "glare/core/assert.h"

#pragma warning(disable:4100)
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

template<typename T>
T from_repr(const string& s, std::enable_if_t<std::is_class_v<T> && !std::is_same_v<T, string>, nullptr_t>* _ = nullptr)
{
	return T::from_repr(s);	
}

template<typename T>
T from_repr(const string& s, std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, nullptr_t>* _ = nullptr)
{
	int64 tmp = std::strtoll(s.c_str(), nullptr, 10);
	return static_cast<T>(tmp);
}

template<typename T>
T from_repr(const string& s, std::enable_if_t<std::is_floating_point_v<T>, nullptr_t>* _ = nullptr)
{
	float32 tmp = std::strtof(s.c_str(), nullptr);
	return static_cast<T>(tmp);
}

template<typename T>
T from_repr(const string& s, std::enable_if_t<std::is_pointer_v<T>, nullptr_t>* _ = nullptr)
{
	uint64 tmp = std::strtoull(s.c_str(), nullptr, 16);
	return reinterpret_cast<T>(tmp);
}

template<typename T>
T from_repr(const string& s, std::enable_if_t<std::is_same_v<T, string>, nullptr_t>* _ = nullptr)
{
	return s;
}

template<typename T>
T from_repr(const string& s, std::enable_if_t<std::is_same_v<T, bool>, nullptr_t>* _ = nullptr)
{
	ASSERT(!s.empty(), "null repr string");
	const char s0 = s[0];
	return s0 == 'T' || s0 == 't' || s0 == '1' || s0 == 'Y' || s0 =='y';
}

};

#pragma warning(default:4100)