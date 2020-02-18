/// glare/core/vector.h
/// Convention
///
///
/// operator + - * > >= < <= == are component-wise
/// float vectors also have a component-wise /
///
///
///

#pragma once
#include "glare/core/common.h"
#include <cmath>
namespace glare
{
struct vec2;
struct vec3;
struct vec4;
struct ivec2;
	//////////////////////////////////////////////////////////////////////////
	//////////      Vector of float numbers    ///////////////////////////////
	//////////////////////////////////////////////////////////////////////////

struct vec2
{
	union { float32 x = 0.f; float32 u; float32 s; };
	union { float32 y = 0.f; float32 v; float32 t; };
	vec2() = default;
	vec2(float32 x, float32 y) :x(x), y(y) {}
	vec2(const ivec2& cast_from);
	vec2(const vec3& truncate_from);
	vec2(const vec4& truncate_from);

	NODISCARD string repl() const;
	
	vec2& operator = (const vec2& copy_from) { x = copy_from.x; y = copy_from.y; return *this; }
	vec2& operator += (const vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
	vec2& operator -= (const vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
	vec2& operator *= (const vec2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
	vec2& operator /= (const vec2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
	vec2& operator *= (float scale) { x *= scale; y *= scale; return *this; }

	void set_length(float32 length);
	void normalize() { set_length(1.f); }
	void rotate_90_deg(int32 positive=1);
	void rotate(float32 radian);
	void rotate_deg(float32 degree);

	vec2 operator + (const vec2& rhs) const { return {x + rhs.x, y + rhs.y}; }
	vec2 operator - (const vec2& rhs) const { return {x - rhs.x, y - rhs.y}; }
	vec2 operator * (const vec2& rhs) const { return {x * rhs.x, y * rhs.y}; }
	vec2 operator / (const vec2& rhs) const { return {x / rhs.x, y / rhs.y}; }
	vec2 operator * (float32 scale) const { return {x * scale, y * scale}; }
	vec2 operator / (float32 inv_scale) const { const float scale = 1.f / inv_scale; return { x * scale, y * scale }; }
	friend vec2 operator * (float32 scale, const vec2& v);

	bool operator == (const vec2& rhs) const { return x == rhs.x && y == rhs.y; }
	bool operator >= (const vec2& rhs) const { return x >= rhs.x && y >= rhs.y; }
	bool operator <= (const vec2& rhs) const { return x <= rhs.x && y <= rhs.y; }
	bool operator > (const vec2& rhs) const { return x > rhs.x && y > rhs.y; }
	bool operator < (const vec2& rhs) const { return x < rhs.x && y < rhs.y; }
	
	NODISCARD float32 dot(const vec2& rhs) const { return x * rhs.x + y * rhs.y; }
	NODISCARD float32 cross2(const vec2& rhs) const { return x * rhs.y - y * rhs.x; }

	NODISCARD float32 length() const { return ::sqrtf(x * x + y * y); }
	NODISCARD float32 length_square() const { return x * x + y * y; }
	NODISCARD vec2 normalized() const { vec2 r = *this; r.normalize(); return r; }
	NODISCARD vec2 ratated_90_deg(int32 positive=1) const
	{
		vec2 result(*this);
		result.rotate_90_deg(positive);
		return result;
	}
	static vec2 ZERO;
	static vec2 ONE;
	static vec2 from_repl(const string& repl);
};

struct vec3
{
	union { float32 x = 0.f; float32 r; };
	union { float32 y = 0.f; float32 g; };
	union { float32 z = 0.f; float32 b; };
	vec3() = default;
	vec3(float32 x, float32 y, float32 z) :x(x), y(y), z(z) {}
	vec3(const vec2& promote_from, float32 z=0.f);
	vec3(const vec4& truncate_from);

	NODISCARD string repl() const;

	vec3& operator =  (const vec3& copy_from) { x = copy_from.x; y = copy_from.y; z = copy_from.z; return *this; }
	vec3& operator += (const vec3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	vec3& operator -= (const vec3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	vec3& operator *= (const vec3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	vec3& operator /= (const vec3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
	vec3& operator *= (float32 scale) { x *= scale; y *= scale; z *= scale; return *this; }

	vec3 operator + (const vec3& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z }; }
	vec3 operator - (const vec3& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z }; }
	vec3 operator * (const vec3& rhs) const { return { x * rhs.x, y * rhs.y, z * rhs.z }; }
	vec3 operator / (const vec3& rhs) const { return { x / rhs.x, y / rhs.y, z / rhs.z }; }
	vec3 operator * (float32 scale) const { return { x * scale, y * scale, z * scale }; }
	vec3 operator / (float32 inv_scale) const { const float scale = 1.f / inv_scale; return { x * scale, y * scale, z * scale }; }
	friend vec3 operator * (float32 scale, const vec3& v);
	
	static vec3 ZERO;
	static vec3 ONE;
	static vec2 from_repl(const string& repl);
};

struct vec4
{
	union { float32 x = 0.f; float32 r; };
	union { float32 y = 0.f; float32 g; };
	union { float32 z = 0.f; float32 b; };
	union { float32 w = 0.f; float32 a; };
	vec4() = default;
	vec4(float32 x, float32 y, float32 z, float32 w) :x(x), y(y), z(z), w(w) {}
	vec4(const vec2& promote_from, float32 z=0.f, float32 w=0.f);
	vec4(const vec3& promote_from, float32 w=0.f);

	NODISCARD string repl() const;

	vec4& operator =  (const vec4& copy_from) { x = copy_from.x; y = copy_from.y; z = copy_from.z; w = copy_from.w; return *this; }
	vec4& operator += (const vec4& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	vec4& operator -= (const vec4& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
	vec4& operator *= (const vec4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
	vec4& operator /= (const vec4& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
	vec4& operator *= (float32 scale) { x *= scale; y *= scale; z *= scale; w *= scale; return *this; }

	vec4 operator + (const vec4& rhs) const { return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
	vec4 operator - (const vec4& rhs) const { return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }
	vec4 operator * (const vec4& rhs) const { return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w }; }
	vec4 operator / (const vec4& rhs) const { return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w }; }
	vec4 operator * (float32 scale) const { return { x * scale, y * scale, z * scale, w * scale }; }
	vec4 operator / (float32 inv_scale) const { const float scale = 1.f / inv_scale; return { x * scale, y * scale, z * scale, w * scale}; }
	friend vec3 operator * (float32 scale, const vec3& v);
	
	static vec4 ZERO;
	static vec4 ONE;
	static vec4 from_repl(const string& repl);
};
	//////////////////////////////////////////////////////////////////////////
	//////////      Vector of integers         ///////////////////////////////
	//////////////////////////////////////////////////////////////////////////

struct ivec2
{
	union { int32 x = 0; int32 u; };
	union { int32 y = 0; int32 v; };

	ivec2() = default;
	ivec2(int32 x, int32 y) :x(x), y(y) {} //implicit OK, allows {1,2} to be an ivec2
	ivec2(const vec2& castFrom) :x(static_cast<int32>(castFrom.x)), y(static_cast<int32>(castFrom.y)) {} //implicit cast OK
	~ivec2() = default;

	NODISCARD string repl() const;
	
	ivec2 operator + (const ivec2& rhs)	const	{ return ivec2{ x + rhs.x, y + rhs.y }; }
	ivec2 operator - (const ivec2& rhs)	const	{ return ivec2{ x - rhs.x, y - rhs.y }; }
	ivec2 operator * (const ivec2& rhs)	const	{ return ivec2{ x * rhs.x, y * rhs.y }; }
	
	bool operator > (const ivec2& rhs)	const	{ return x > rhs.x && y > rhs.y; }
	bool operator < (const ivec2& rhs)	const	{ return x < rhs.x && y < rhs.y; }
	bool operator >= (const ivec2& rhs)	const	{ return x >= rhs.x && y <= rhs.y; }
	bool operator <= (const ivec2& rhs)	const	{ return x <= rhs.x && y <= rhs.y; }
	bool operator == (const ivec2& rhs)	const	{ return x == rhs.x && y == rhs.y; }
	bool operator != (const ivec2& rhs)	const	{ return !(this->operator==(rhs)); }

	ivec2& operator = (const ivec2& copyFrom)	{ x = copyFrom.x; y = copyFrom.y; return *this; }
	ivec2& operator += (const ivec2& rhs)		{ x += rhs.x; y += rhs.y; return *this; }
	ivec2& operator -= (const ivec2& rhs)		{ x -= rhs.x; y -= rhs.y; return *this; }
	ivec2& operator *= (const ivec2& rhs)		{ x *= rhs.x; y *= rhs.y; return *this; }

	static vec2 from_repl(const string& repl);
	
};
};