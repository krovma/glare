#pragma once
#include "glare/core/common.h"

namespace glare
{

#if defined(__cpp_lib_math_constants)
#include <numbers>
constexpr float32 PI = std::numbers:pi_v<float32>;
constexpr float32 SQRT_2 = std::numbers::sqrt2_v<float32>
#else
constexpr float32 PI = static_cast<float32>(3.14159265358979323846);
constexpr float32 SQRT_2 = static_cast<float32>(1.41421356237309504880);
#endif

template<typename T>
constexpr decltype(auto) clamp(const T& value, const T& min, const T& max)
{
	return value < min ? min : (value > max ? max : value);
}

template<typename T>
constexpr int32 sgn(const T& value)
{
	const T zero(0);
	return static_cast<int32>(zero < value) - static_cast<int32>(value < zero);
}

template<typename T, typename P>
constexpr decltype(auto) lerp(const T& a, const T& b, const P alpha)
{
	return (static_cast<P>(1.f) - alpha)* a + (alpha * b);
}

inline float32 rtd(float32 rad) { return rad / PI * 180.f; }
inline float32 dtr(float32 deg) { return deg / 180.f * PI; }

inline float32 cos_deg(float32 deg) { return cosf(dtr(deg)); }
inline float32 sin_deg(float32 deg) { return sinf(dtr(deg)); }

};