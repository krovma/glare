#include "glare/math/vector.h"

////////////////////////////////
namespace glare {

vec2& vec2::operator+=(const vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
	return *this;
}

};