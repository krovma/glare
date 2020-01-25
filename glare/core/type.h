#pragma once

#if GLARE_USE_STD_STRING
#include <string>
#endif

namespace glare
{
using int32  =	long int;
using uint32 =	unsigned long int;
using int16  =	short int;
using uint16 =	unsigned short int;
using int64  =	long long int;
using uint64 =	unsigned long long int;
using int8   =	char;
using uint8  =	unsigned char;
using byte   =	unsigned char;

using float32 = float;
using float64 = double;

#if GLARE_USE_STD_STRING
using string = std::string;
#else
//using string = g
#endif
};