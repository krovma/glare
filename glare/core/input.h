#pragma once
#include "glare/core/common.h"
namespace glare
{
constexpr byte KEY_LEFT_ARROW		= 0x25; //VK_LEFT
constexpr byte KEY_UP_ARROW			= 0x26;	//VK_UP
constexpr byte KEY_RIGHT_ARROW		= 0x27; //VK_RIGHT
constexpr byte KEY_DOWN_ARROW		= 0x28; //VK_DOWN

constexpr byte KEY_SHIFT			= 0x10; //VK_SHIFT

constexpr byte KEY_F1				= 0x70;
constexpr byte KEY_F2				= 0x71;
constexpr byte KEY_F3				= 0x72;

namespace keyboard
{
	void set_key_state(byte keycode, bool pressed);
	bool get_key_state(byte keycode);
}
}