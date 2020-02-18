#pragma once
#include "glare/core/common.h"
#include "glare/core/color.h"
#include "glare/math/vector.h"
#include <vector>
#include <unordered_map>

namespace glare
{
class surface
{
public:
	static surface* load_image(const string& id, const char* path, bool flip_v=false);
	surface() = default;
	surface(const char* path, bool flip_v=false);
	surface(size_t w, size_t h, const rgba& default_color=color::WHITE);
	surface(size_t w, size_t h, const rgba* texel_data);
	~surface();
	NODISCARD const rgba get_texel(const ivec2& texel_coord) const;
	void set_texel(const ivec2& texel_coord, const rgba& color);
	NODISCARD rgba* get_surface_buffer();
	NODISCARD const rgba* get_surface_buffer() const;

	void write_png(const char* path) const;
public:
	string	m_path;
	ivec2	m_size;
	std::vector<rgba>	m_surface_data;
	mutable byte*		m_raw = nullptr;
	uint8	m_raw_channels = 4;
public:
	static std::unordered_map<string, surface*> s_cached;
};
}
