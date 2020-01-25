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
namespace glare
{
	//////////////////////////////////////////////////////////////////////////
	//////////      Vector of float numbers    ///////////////////////////////
	//////////////////////////////////////////////////////////////////////////
struct vec2
{
	union { float32 x = 0.f; float32 u; float32 s; };
	union { float32 y = 0.f; float32 v; float32 t; };
	vec2() = default;

	vec2& operator += (const vec2& rhs);
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
	~ivec2() {}

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

};
};