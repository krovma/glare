#pragma once
#include "glare/core/common.h"
namespace glare
{
struct vec2
{
	union { float32 x = 0.f; float32 u; float32 s; };
	union { float32 y = 0.f; float32 v; float32 t; };
	vec2() = default;

	vec2& operator += (const vec2& rhs);
};
};