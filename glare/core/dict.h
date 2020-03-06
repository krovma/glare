#pragma once
#include "glare/core/common.h"
#include "glare/core/string_utils.h"
#include <unordered_map>

#include "glare/math/vector.h"

namespace glare
{
class entry_base
{
public:
	virtual ~entry_base() = default;
	virtual string repr() = 0;
};

template<typename T>
class dict_entry : public entry_base
{
public:
	dict_entry(T value)
		: m_value(std::move(value))
	{
	}
	string repr() override
	{
		return glare::repr(m_value);
	}

public:
	T m_value;
};

class dict
{
public:
	template<typename T>
	void set(const string& k, const T& v)
	{
		auto found = m_dict.find(k);
		if (found != std::end(m_dict)) {
			delete found->second;
		}
		m_dict[k] = new dict_entry<T>(v);
	}

	void set(const string&k, const char* v)
	{
		set(k, string(v));
	}
	
	template<typename T>
	T get(const string& k, const T& def)
	{
		auto found = m_dict.find(k);
		if (found != std::end(m_dict)) {
			dict_entry<T>* p = dynamic_cast<dict_entry<T>*>(found->second);
			if (!p) {
				return def;
			}
			return p->m_value;
		}
		return def;
	}

	string get(const string& k, const char* def)
	{
		return get(k, string(def));
	}
	
public:
	std::unordered_map<string, entry_base*> m_dict;
};

}