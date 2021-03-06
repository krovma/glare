#include "glare/math/vector.h"
#include "glare/math/utilities.h"
#include "glare/core/string_utils.h"
#include "glare/core/assert.h"
#include <cstdlib>
////////////////////////////////
namespace glare {
STATIC const ivec2 ivec2::ZERO;
STATIC const vec2 vec2::ZERO;
STATIC const vec3 vec3::ZERO;
STATIC const vec4 vec4::ZERO;
STATIC const ivec2 ivec2::ONE {1, 1};
STATIC const vec2 vec2::ONE { 1, 1 };
STATIC const vec3 vec3::ONE { 1, 1, 1 };
STATIC const vec4 vec4::ONE { 1, 1, 1, 1 };

vec2::vec2(const ivec2& cast_from)
    : x(static_cast<float32>(cast_from.x))
	, y(static_cast<float32>(cast_from.y))
{
}

vec2::vec2(const vec3& truncate_from)
	: x(truncate_from.x)
	, y(truncate_from.y)
{
}

vec2::vec2(const vec4& truncate_from)
	: x(truncate_from.x)
	, y(truncate_from.y)
{
}

string vec2::repr() const
{
	return format("%g,%g", x, y);
}

void vec2::set_length(float32 new_length)
{
	const float32 old_length = length();
	if (old_length == 0.f) { // No tolerance
		x = new_length;
		y = 0.f;
	} else {
		const float scale = new_length / old_length;
		operator*=(scale);
	}
}

void vec2::rotate_90_deg(int32 positive/*=1*/)
{
	const float32 tmp = x;
	x = (positive > 0) ? -y : y;
	y = (positive > 0) ? tmp : -tmp;
}

void vec2::rotate(float32 radian)
{
	const float cc = cos(radian);
	const float ss = sin(radian);
	const float new_x = cc * x - ss * y;
	const float new_y = ss * x + cc * y;
	x = new_x;
	y = new_y;
}

void vec2::rotate_deg(float32 degree)
{
	rotate(dtr(degree));
}

STATIC vec2 vec2::from_repr(const string& repr)
{
	std::vector<string> comps = split(repr.c_str(), ',');
	if (comps.size() < 2) {
		FATAL(format("Cannot convert [%s] to vec2", repr.c_str()));
	}
	if (comps.size() > 2) {
		ALERT(format("[%s] is too long for vec2. Truncated", repr.c_str()));
	}
	vec2 result;
	result.x = strtof(comps[0].c_str(), nullptr);
	result.y = strtof(comps[1].c_str(), nullptr);
	return result;
}

vec3::vec3(const vec2& promote_from, float32 z)
	: x(promote_from.x)
	, y(promote_from.y)
	, z(z)
{
}

vec3::vec3(const vec4& truncate_from)
	: x(truncate_from.x)
	, y(truncate_from.y)
	, z(truncate_from.z)
{
}

string vec3::repr() const
{
	return format("%g,%g,%g", x, y, z);
}

vec2 vec3::from_repr(const string& repl)
{
	std::vector<string> comps = split(repl.c_str(), ',');
	if (comps.size() < 3) {
		FATAL(format("Cannot convert [%s] to vec3", repl.c_str()));
	}
	if (comps.size() > 3) {
		ALERT(format("[%s] is too long for vec3. Truncated", repl.c_str()));
	}
	vec3 result;
	result.x = strtof(comps[0].c_str(), nullptr);
	result.y = strtof(comps[1].c_str(), nullptr);
	result.z = strtof(comps[2].c_str(), nullptr);
	return result;
}

vec4::vec4(const vec2& promote_from, float32 z, float32 w)
	: x(promote_from.x)
	, y(promote_from.y)
	, z(z)
	, w(w)
{
}

vec4::vec4(const vec3& promote_from, float32 w)
	: x(promote_from.x)
	, y(promote_from.y)
	, z(promote_from.z)
	, w(w)
{
}

string vec4::repr() const
{
	return format("%g,%g,%g", x, y, z, w);
	
}

vec4 vec4::from_repr(const string& repr)
{
	std::vector<string> comps = split(repr.c_str(), ',');
	if (comps.size() < 4) {
		FATAL(format("Cannot convert [%s] to vec4", repr.c_str()));
	}
	if (comps.size() > 4) {
		ALERT(format("[%s] is too long for vec4. Truncated", repr.c_str()));
	}
	vec4 result;
	result.x = strtof(comps[0].c_str(), nullptr);
	result.y = strtof(comps[1].c_str(), nullptr);
	result.z = strtof(comps[2].c_str(), nullptr);
	result.w = strtof(comps[3].c_str(), nullptr);
	return result;
}

string ivec2::repr() const
{
	return format("%i,%i", x, y);
	
}

ivec2 ivec2::from_repr(const string& repr)
{
	std::vector<string> comps = split(repr.c_str(), ',');
	if (comps.size() < 2) {
		FATAL(format("Cannot convert [%s] to ivec2", repr.c_str()));
	}
	if (comps.size() > 2) {
		ALERT(format("[%s] is too long for ivec2. Truncated", repr.c_str()));
	}
	ivec2 result;
	result.x = strtol(comps[0].c_str(), nullptr, 10);
	result.y = strtol(comps[1].c_str(), nullptr, 10);
	return result;
}

vec3 operator*(float32 scale, const vec3& v)
{
	return v * scale;
}

vec2 operator*(float32 scale, const vec2& v)
{
	return v * scale;
}
};