#pragma once
#include "glare/core/common.h"
#include "imgui/imgui.h"

namespace glare
{
class window;
class renderer;
STATIC class dev_ui
{
public:
	static void start(window* w, renderer* r);
	static void begin_frame();
	static void update(float32 delta_seconds);
	static void render();
	static void end_frame();
	static void stop();
public:
	static window*		m_window;
	static renderer*	m_renderer;
	static bool			m_run;
};
}