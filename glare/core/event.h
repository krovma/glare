#pragma once
#include "glare/core/common.h"
#include "glare/core/dict.h"
#include <functional>
#include <vector>

namespace glare
{
using event_cb = bool(dict&);

class event_func
{
public:
	event_func(event_cb* callback) 
	{
		m_object = nullptr;
		m_function = reinterpret_cast<void*>(callback);
		m_callable = callback;
	}

	template<typename T>
	event_func(T* obj, bool (T::*callback) (dict&))
	{
		m_object = obj;
		m_function = *(reinterpret_cast<void**>(&callback));
		m_callable = [=] (dict& args) {
			return (obj->*callback)(args);
		};
	}

	event_func(const event_func& copy) = default;
	// 	: m_object(copy.m_object)
	// 	, m_function(copy.m_function)
	// 	, m_callable(copy.m_callable)
	// {
	// }

	NODISCARD bool operator==(const event_func& rhs) const
	{
		return (m_object == rhs.m_object) && (m_function == rhs.m_function);
	}

	bool operator()(dict& args) const
	{
		return m_callable(args);
	}
	
public:
	void* m_object = nullptr;
	const void* m_function = nullptr;
	std::function<event_cb> m_callable;
};

namespace event
{
	using subscriber_list = std::vector<event_func>;
	extern std::unordered_map<string, subscriber_list*> g_event_map;
	void restart();
	void subscribe(const string& id, const event_func& func);
	void unsubscribe(const string& id, const event_func& func);
	void fire(const string& id, dict& args);
}

}

#define EVENT_PROC(name) \
bool name(glare::dict& args)