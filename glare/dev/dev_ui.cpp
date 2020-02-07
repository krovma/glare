#include "glare/dev/dev_ui.h"
#include "glare/core/window.h"
#include "glare/render/renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace glare
{
STATIC window* dev_ui::m_window = nullptr;
STATIC renderer* dev_ui::m_renderer = nullptr;
STATIC bool dev_ui::m_run = false;
STATIC void dev_ui::start(window* w, renderer* r)
{
	m_window = w;
	m_renderer = r;
	m_run = true;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(m_window->m_hwnd);
	ImGui_ImplDX11_Init(m_renderer->get_dx_device(), m_renderer->get_dx_context());
	ImGui::StyleColorsClassic();
}

STATIC void dev_ui::begin_frame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

STATIC void dev_ui::update(float32 delta_seconds)
{
	UNUSED(delta_seconds);
}

STATIC void dev_ui::render()
{
	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();
	render_target* rt = m_renderer->m_frame_render_target;
	dx_rtv* rtv = rt->get_dx_handle();
	m_renderer->get_dx_context()->OMSetRenderTargets(1, &rtv, nullptr);
	ImGui_ImplDX11_RenderDrawData(draw_data);
}

STATIC void dev_ui::end_frame()
{
	ImGui::EndFrame();
}

STATIC void dev_ui::stop()
{
	m_run = false;
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
}
