#include "glare/core/event.h"

namespace glare::event
{
std::unordered_map<string, subscriber_list*> g_event_map;

void restart()
{
	for (auto& each_pair : g_event_map) {
		delete each_pair.second;
	}
	g_event_map.clear();
}

void subscribe(const string& id, const event_func& func)
{
	const auto found = g_event_map.find(id);
	if (found == std::end(g_event_map)) {
		subscriber_list* list = new subscriber_list();
		list->emplace_back(func);
		g_event_map[id] = list;
	} else {
		found->second->emplace_back(func);
	}
}

void unsubscribe(const string& id, const event_func& func)
{
	const auto found = g_event_map.find(id);
	if (found != std::end(g_event_map)) {
		subscriber_list* list = found->second;
		for (auto it = std::begin(*list); it != std::end(*list);) {
			if (*it == func) {
				it = list->erase(it);
			} else {
				++it;
			}
		}
	}
}

void fire(const string& id, dict& args)
{
	const auto found = g_event_map.find(id);
	if (found != std::end(g_event_map)) {
		subscriber_list* list = found->second;
		for (auto& each : *list) {
			each(args);
		}
	}
}

}