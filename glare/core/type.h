#pragma once

#if GLARE_USE_STD_STRING
#include <string>
#endif

#include <cstdint>

namespace glare
{
using int32  =	std::int32_t;
using uint32 =	std::uint32_t;
using int16  =	std::int16_t;
using uint16 =	std::uint16_t;
using int64  =	std::int64_t;
using uint64 =	std::uint64_t;
using int8   =	std::int8_t;
using uint8  =	std::uint8_t;
using byte   =	uint8;

using float32 = float;
using float64 = double;

#if GLARE_USE_STD_STRING
using string = std::string;
#else
//using string = gstring
#endif
};