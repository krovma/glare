#pragma once
#include "glare/core/common.h"
namespace glare
{
struct rgba;
struct rgba8;

struct rgba8
{
	byte r = 255;
	byte g = 255;
	byte b = 255;
	byte a = 255;
	rgba8() = default;
	explicit rgba8(byte r, byte g, byte b, byte a = 255)
		:r(r), g(g), b(b), a(a) {
	}
	rgba8(const rgba8& copy_from)
		:r(copy_from.r), g(copy_from.g), b(copy_from.b), a(copy_from.a)
	{}
	rgba8& operator=(const rgba8& copy_from)
	{
		r = copy_from.r;
		g = copy_from.g;
		b = copy_from.b;
		a = copy_from.a;
		return *this;
	}
	rgba8(const rgba& float_color);
};

struct rgba
{
	float32 r = 1;
	float32 g = 1;
	float32 b = 1;
	float32 a = 1;

	rgba() = default;
	explicit rgba(float32 r, float32 g, float32 b, float32 a=1.f)
		:r(r), g(g), b(b), a(a) {	
	}
	rgba(const rgba8& byte_color);
};

namespace color
{
extern const rgba WHITE;		//0xFFFFFF FF(by Default)
extern const rgba BLACK;		//0x000000
extern const rgba RED;			//0xFF0000
extern const rgba LIME;			//0x00FF00
extern const rgba BLUE;			//0x0000FF
extern const rgba CYAN;			//0x00FFFF
extern const rgba MAGENTA;		//0xFF00FF
extern const rgba YELLOW;		//0xFFFF00
extern const rgba SILVER;		//0xC0C0C0
extern const rgba GRAY;			//0x808080
extern const rgba MAROON;		//0x800000
extern const rgba OLIVE;		//0x808000
extern const rgba GREEN;		//0x008000
extern const rgba PURPLE;		//0x800080
extern const rgba TEAL;			//0x008080
extern const rgba NAVY;			//0x000080

extern const rgba TRANSPARENT_BLACK;	//0x00000000
extern const rgba TRANSPARENT_WHITE;	//0xFFFFFF00

extern const rgba FLAT;
};

};