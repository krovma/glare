#include "glare/core/color.h"

namespace glare
{
/////////////////////////////
///////RGBA8
rgba8::rgba8(const rgba& float_color)
{
	r = static_cast<byte>(float_color.r * 255.f);
	g = static_cast<byte>(float_color.g * 255.f);
	b = static_cast<byte>(float_color.b * 255.f);
	a = static_cast<byte>(float_color.a * 255.f);
}


/////////////////////////////
////RGBA
rgba::rgba(const rgba8& byte_color)
{
	r = static_cast<float32>(byte_color.r) / 255.f;
	g = static_cast<float32>(byte_color.g) / 255.f;
	b = static_cast<float32>(byte_color.b) / 255.f;
	a = static_cast<float32>(byte_color.a) / 255.f;
}

namespace color
{
const rgba WHITE	= rgba(1.f, 1.f, 1.f);	//0xFFFFFF FF(by Default)
const rgba BLACK	= rgba(0.f, 0.f, 0.f);	//0x000000
const rgba RED		= rgba(1.f, 0.f, 0.f);	//0xFF0000
const rgba LIME		= rgba(0.f, 1.f, 0.f);	//0x00FF00
const rgba BLUE		= rgba(0.f, 0.f, 1.f);	//0x0000FF
const rgba CYAN		= rgba(0.f, 1.f, 1.f);	//0x00FFFF
const rgba MAGENTA	= rgba(1.f, 0.f, 1.f);	//0xFF00FF
const rgba YELLOW	= rgba(1.f, 1.f, 0.f);	//0xFFFF00
const rgba SILVER	= rgba(75.f, 75.f, .75f);	//0xC0C0C0
const rgba GRAY		= rgba(.5f, .5f, .5f);	//0x808080
const rgba MAROON	= rgba(.5f, 0.f, 0.f);	//0x800000
const rgba OLIVE	= rgba(.5f, .5f, 0.f);	//0x808000
const rgba GREEN	= rgba(0.f, .5f, 0.f);	//0x008000
const rgba PURPLE	= rgba(.5f, 0.f, .5f);	//0x800080
const rgba TEAL		= rgba(0.f, .5f, .5f);	//0x008080
const rgba NAVY		= rgba(0.f, 0.f, .5f);	//0x000080

const rgba TRANSPARENT_BLACK = rgba(0.f, 1.f, 0.f);	//0x00000000
const rgba TRANSPARENT_WHITE = rgba(0.f, 1.f, 0.f);	//0xFFFFFF00

const rgba FLAT = rgba(.5f, .5f, 1.f);
};
};
