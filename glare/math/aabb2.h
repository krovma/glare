#pragma once
#include "glare/core/common.h"
#include "glare/math/vector.h"

namespace glare
{
struct aabb2
{
	vec2 min;
	vec2 max;

	aabb2() = default;
	aabb2(const vec2& min, const vec2& max)
		: min(min), max(max)
	{}
	aabb2(float32 min_x, float32 min_y, float32 max_x, float32 max_y)
		: min(min_x, min_y), max(max_x, max_y)
	{}
	
	NODISCARD vec2 get_extends() const
	{
		const vec2 size = max - min;
		return size * 0.5f;
	}

	NODISCARD vec2 get_size() const { return max - min; }

	NODISCARD vec2 get_center() const
	{
		return (min + max) * 0.5f;
	}

	static const aabb2 UNIT;
};
}