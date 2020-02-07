#pragma once
#include "glare/core/common.h"
#include "pugixml/pugixml.hpp"

namespace glare
{
namespace xml
{
using node = pugi::xml_node;
using document = pugi::xml_document;
using attribute = pugi::xml_attribute;
using xpath_query = pugi::xpath_query;

document*	load_file(const char* path);
void		unload_file(document*& doc);

// Attributes
template <typename T>
T get_attr(node& node, const char* attr_name, const T& default_value)
{
	T value = default_value;
	const attribute attr = node.attribute(attr_name);
	if (!attr.empty()) {
		string repl = attr.value();
		value = T::from_repl(repl);
	}
	return value;
}

string get_attr(node& node, const char* attr_name, const char* default_value);

template<>
string get_attr(node& node, const char* attr_name, const string& default_value);

template<>
int32 get_attr(node& node, const char* attr_name, const int32& default_value);

template<>
float32 get_attr(node& node, const char* attr_name, const float32& default_value);

template<>
bool get_attr(node& node, const char* attr_name, const bool& default_value);

template<>
char get_attr(node& node, const char* attr_name, const char& default_value);

template<>
uint32 get_attr(node& node, const char* attr_name, const uint32& default_value);

};
};