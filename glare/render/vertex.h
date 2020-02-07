#pragma once
#include "glare/math/vector.h"
#include "glare/core/color.h"
#include "glare/render/buffer.h"

namespace glare
{
struct super_vertex
{
	vec3 position;
	rgba color;
	vec2 uv;
};

struct vertex_pcu
{
	vec3 position;
	rgba color = color::WHITE;
	vec2 uv;

	vertex_pcu() = default;
	vertex_pcu(const vec3& position, const rgba& color, const vec2& uv)
		:position(position), color(color), uv(uv)
	{
	}
	vertex_pcu(const vertex_pcu& copy) = default;
	vertex_pcu& operator= (const vertex_pcu& copy)
	{
		position = copy.position;
		color = copy.color;
		uv = copy.uv;
		return *this;
	}
	
	static buffer_layout::attribute _s_attr[];
	static void _copy_from_super_vertex(void* dst, const super_vertex* src, size_t count);
};

inline void vertex_pcu::_copy_from_super_vertex(void* dst, const super_vertex* src, size_t count)
{
	vertex_pcu* buffer = static_cast<vertex_pcu*>(dst);
	for (size_t i = 0; i < count; ++i) {
		buffer[i].position = src[i].position;
		buffer[i].color = src[i].color;
		buffer[i].uv = src[i].uv;
	}
}
};