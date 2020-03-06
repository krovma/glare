#include "glare/core/input.h"

namespace glare
{
static bool keyboard_status[256];

namespace keyboard{
	void set_key_state(byte keycode, bool pressed)
	{
		keyboard_status[keycode] = pressed;
	}
	bool get_key_state(byte keycode)
	{
		return keyboard_status[keycode];
	}
}
}
