#include "glare/data/xml_utils.h"
#include "glare/core/assert.h"
#include "glare/core/string_utilities.h"

namespace glare
{
namespace xml
{
document* load_file(const char* path)
{
	document* doc = new pugi::xml_document();
	const pugi::xml_parse_result parse_result = doc->load_file(path);
	if (parse_result.status != pugi::status_ok) {
		FATAL(format("Loading xml data failed. (%s)", parse_result.description()));
	}
	return doc;
}

void unload_file(document*& doc)
{
	if (!doc) {
		return;
	}
	delete doc;
	doc = nullptr;
}
	
string get_attr(const node& node, const char* attr_name, const char* default_value)
{
	const attribute attr = node.attribute(attr_name);
	return attr.as_string(default_value);
}

template<>
string get_attr(const node& node, const char* attr_name, const string& default_value)
{
	const attribute attr = node.attribute(attr_name);
	return attr.as_string(default_value.c_str());

}

template <>
int32 get_attr(const node& node, const char* attr_name, const int32& default_value)
{
	const attribute attr = node.attribute(attr_name);
	return attr.as_int(default_value);
}

template <>
float32 get_attr(const node& node, const char* attr_name, const float32& default_value)
{
	const attribute attr = node.attribute(attr_name);
	return attr.as_float(default_value);
}

//Pugi docs: https://pugixml.org/docs/manual.html#access.attrdata
//| as_bool converts attribute value to boolean as follows: if attribute handle is null, def argument is returned (which is false by default).
//| If attribute value is empty, false is returned. Otherwise, true is returned if the first character is one of '1', 't', 'T', 'y', 'Y'.
//| This means that strings like "true" and "yes" are recognized as true, while strings like "false" and "no" are recognized as false.
//| For more complex matching you¡¯ll have to write your own function.
template <>
bool get_attr(const node& node, const char* attr_name, const bool& default_value)
{
	const attribute attr = node.attribute(attr_name);
	return attr.as_bool(default_value);
}

template <>
char get_attr(const node& node, const char* attr_name, const char& default_value)
{
	const attribute attr = node.attribute(attr_name);
	if (attr.empty()) {
		return default_value;
	}
	return attr.value()[0];
}

template <>
uint32 get_attr(const node& node, const char* attr_name, const uint32& default_value)
{
	const attribute attr = node.attribute(attr_name);
	return attr.as_uint(default_value);
}
};
};