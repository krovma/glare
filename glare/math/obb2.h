#pragma once
#include "glare/core/common.h"
#include "glare/math/vector.h"
#include "glare/math/aabb2.h"
#include "glare/math/utilities.h"

namespace glare
{
struct obb2
{
public:
	vec2 center = vec2::ZERO;
	vec2 right = {1.f, 0.f};
	vec2 extends = vec2::ZERO;
public:
	obb2() = default;
	obb2(const vec2& center, const vec2& size = vec2::ZERO, float rot_deg = 0.f)
		: center(center)
		, extends (size * 0.5f)
	{
		right = {cos_deg(rot_deg), sin_deg(rot_deg)};
	}
	obb2(const obb2& copy)
		: center(copy.center)
		, right(copy.right)
		, extends(copy.extends)
	{
	}

	explicit obb2(const aabb2& convert_from)
	{
		center = convert_from.get_center();
		extends = convert_from.get_extends();
	}

	obb2 operator+(const vec2& move_by) const
	{
		obb2 result(*this);
		result.center += move_by;
		return result;
	}
	void move(const vec2& move_by)
	{
		center += move_by;
	}
	void rotate(float rot_deg)
	{
		right.rotate(dtr(rot_deg));
	}
	void set_position(const vec2& position)
	{
		center = position;
	}
	void set_rotation(float rot_deg)
	{
		right = {cos_deg(rot_deg), sin_deg(rot_deg)};
	}

	NODISCARD vec2 topleft()	const	{ return center - extends.x * right + extends.y * right.ratated_90_deg(); }
	NODISCARD vec2 topright()	const	{ return center + extends.x * right + extends.y * right.ratated_90_deg(); }
	NODISCARD vec2 bottomleft()	const	{ return center - extends.x * right - extends.y * right.ratated_90_deg(); }
	NODISCARD vec2 bottomright()const	{ return center + extends.x * right - extends.y * right.ratated_90_deg(); }
	NODISCARD vec2 size() const { return extends * 2.f; }


	vec2 world_to_local(const vec2& world_pos) const;
	vec2 local_to_world(const vec2& local_pos) const;

	vec2 get_nearest_point	(const vec2& worldPoint) const;
	bool is_overlapping		(const obb2& with) const;
	bool is_overlapping		(const vec2& worldPosition) const;

	aabb2 get_bounding() const;

	// Output corners in order of [tl, tr, bl, br]
	void get_corners(vec2* out) const
	{
		out[0] = topleft();
		out[1] = topright();
		out[2] = bottomleft();
		out[3] = bottomright();
	}
};
}