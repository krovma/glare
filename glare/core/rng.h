#pragma once
#include "glare/core/common.h"

namespace glare
{
STATIC class noise
{
// License
/////////////////////////////////////////////////////////////////////////////////////////////////
// SquirrelNoise4 - Squirrel's Raw Noise utilities (version 4)
//
// This code is made available under the Creative Commons attribution 3.0 license (CC-BY-3.0 US):
//	Attribution in source code comments (even closed-source/commercial code) is sufficient.
//	License summary and text available at: https://creativecommons.org/licenses/by/3.0/us/
//
// These noise functions were written by Squirrel Eiserloh as a cheap and simple substitute for
//	the [sometimes awful] bit-noise sample code functions commonly found on the web, many of which
//	are hugely biased or terribly patterned, e.g. having bits which are on (or off) 75% or even
//	100% of the time (or are excessively overkill/slow for our needs, such as MD5 or SHA).
//
// Note: This is work in progress, and has not yet been tested thoroughly.  Use at your own risk.
//	Please report any bugs, issues, or bothersome cases to SquirrelEiserloh at gmail.com.
//
// The following functions are all based on a simple bit-noise hash function which returns an
//	unsigned integer containing 32 reasonably-well-scrambled bits, based on a given (signed)
//	integer input parameter (position/index) and [optional] seed.  Kind of like looking up a
//	value in an infinitely large [non-existent] table of previously rolled random numbers.
//
// These functions are deterministic and random-access / order-independent (i.e. state-free),
//	so they are particularly well-suited for use in smoothed/fractal/simplex/Perlin noise
//	functions and out-of-order (or or-demand) procedural content generation (i.e. that mountain
//	village is the same whether you generated it first or last, ahead of time or just now).
//
// The N-dimensional variations simply hash their multidimensional coordinates down to a single
//	32-bit index and then proceed as usual, so while results are not unique they should
//	(hopefully) not seem locally predictable or repetitive.
//
/////////////////////////////////////////////////////////////////////////////////////////////////
public:
	static constexpr uint32 uint1d(uint32 x, uint32 seed = 0)
	{
		constexpr uint32 BIT_NOISE1 = 0xd2a80a23;
		constexpr uint32 BIT_NOISE2 = 0xa884f197;
		constexpr uint32 BIT_NOISE3 = 0x1b56c4e9;

		uint32 mangledBits = x;
		mangledBits *= BIT_NOISE1;
		mangledBits += seed;
		mangledBits ^= (mangledBits >> 7);
		mangledBits += BIT_NOISE2;
		mangledBits ^= (mangledBits >> 8);
		mangledBits *= BIT_NOISE3;
		mangledBits ^= (mangledBits >> 11);
		return mangledBits;
	}

	static constexpr float32 float_zero_one(uint32 x, uint32 seed = 0)
	{
		constexpr float64 ONE_OVER_MAX_UINT = 1.0 / static_cast<float64>(0xFFFFFFFF);
		return static_cast<float>(ONE_OVER_MAX_UINT * static_cast<float64>(uint1d(x, seed)));
	}

	static constexpr float32 float_neg_one_one(uint32 x, uint32 seed = 0)
	{
		constexpr float64 ONE_OVER_MAX_INT = 1.0 / static_cast<float64>(0xFFFFFFFF);
		return static_cast<float>(
			ONE_OVER_MAX_INT * 
			static_cast<float64>(static_cast<int32>(uint1d(x, seed)))
		);
	}
};

class rng
{
public:
	rng() = default;
	rng(uint32 seed, uint32 offset=0)
	{
		init(seed, offset);
	}

	void init(uint32 seed, uint32 offset)
	{
		m_seed = seed;
		m_offset = offset;
	}

	int32 next_int(int32 max_include)
	{
		return rand() % max_include;
	}

	float32 next_float(float32 max_include)
	{
		return rand_normal() * max_include;
	}

	float32 rand_normal()
	{
		return noise::float_zero_one(m_offset++, m_seed);
	}
	
	float32 rand_noise()
	{
		return noise::float_neg_one_one(m_offset++, m_seed);
	}

	int32 rand()
	{
		return abs(static_cast<int>(noise::uint1d(m_offset++, m_seed)));
	}

public:
	uint32 m_seed = 0;
	uint32 m_offset = 0;
};
}