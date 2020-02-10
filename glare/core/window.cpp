#include "glare/core/window.h"
#include "glare/core/assert.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if _WIN32 || _WIN64
namespace glare
{
windows_event_callback_t* DEFAULT_WINDOW_EVENT_PROC = nullptr;
static constexpr const TCHAR* __GAME_WINDOW_CLASS_NAME = TEXT("glare_game_window");
static int __window_class_registry_count = 0;

static void __lock_mouse_to_window(HWND hwnd);
static void __unlock_mouse();
static LRESULT CALLBACK __glare_windows_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
static void __register_game_window_class();
static void __unregister_game_window_class();

//////////////////////////////////////////////////////////////////////////
static void __register_game_window_class()
{
	WNDCLASSEX window_class_desc;
	memset(&window_class_desc, 0, sizeof(WNDCLASSEX));
	window_class_desc.cbSize = sizeof(WNDCLASSEX);
	window_class_desc.style = CS_OWNDC;
	window_class_desc.lpfnWndProc = static_cast<WNDPROC>(__glare_windows_window_proc);
	window_class_desc.hInstance = GetModuleHandle(nullptr);
	window_class_desc.hIcon = nullptr; //#Todo: Add Icon and Cursor binding
	window_class_desc.hCursor = nullptr;
	window_class_desc.lpszClassName = __GAME_WINDOW_CLASS_NAME;
	RegisterClassEx(&window_class_desc);
}

//////////////////////////////////////////////////////////////////////////
void __unregister_game_window_class()
{
	UnregisterClass(__GAME_WINDOW_CLASS_NAME, GetModuleHandle(nullptr));
}

//////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK __glare_windows_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
{
	window* const p_window = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch (message) {
	case WM_ACTIVATE: {
		const WORD active_state = LOWORD(w_param);
		const bool is_active = (active_state == WA_ACTIVE || active_state == WA_CLICKACTIVE);
		if (is_active && p_window->is_mouse_locked()) {
			__lock_mouse_to_window(hwnd);
		}
		const bool is_inactive = (active_state == WA_INACTIVE);
		if (is_inactive && p_window->is_mouse_locked()) {
			__unlock_mouse();
		}
		break;
	}
	default: { }
	}
	
	bool message_consumed = false;
	if (p_window) {
		message_consumed = p_window->game_window_proc(hwnd, message, w_param, l_param);
	}
	if (!message_consumed) {
		return DefWindowProc(hwnd, message, w_param, l_param);
	} else {
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
static void __lock_mouse_to_window(HWND hwnd)
{
	RECT client_rect;
	::GetClientRect(hwnd, &client_rect);
	POINT client_topleft{ 0, 0 };
	::ClientToScreen(hwnd, &client_topleft);
	OffsetRect(&client_rect, client_topleft.x, client_topleft.y);
	::ClipCursor(&client_rect);
}

//////////////////////////////////////////////////////////////////////////
static void __unlock_mouse()
{
	::ClipCursor(nullptr);
}

////////////////////////////////
window::window()
	: game_window_proc(DEFAULT_WINDOW_EVENT_PROC)
{
	if (__window_class_registry_count <= 0) {
		__register_game_window_class();
	}
	++__window_class_registry_count;
}

////////////////////////////////
window::~window()
{
	destroy();
	--__window_class_registry_count;
	if (__window_class_registry_count <= 0) {
		__unregister_game_window_class();
	}
}

////////////////////////////////
bool window::is_open() const
{
	return (m_hwnd != nullptr);
}

////////////////////////////////
void window::create(const string& title, float32 aspect_ratio, float32 client_desktop_fraction, windows_event_callback_t* event_proc)
{
	ASSERT(!is_open(), "Window has already opened.");

	const DWORD window_style_flags = WS_CAPTION /*| WS_BORDER | WS_THICKFRAME*/ | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD window_style_ex = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktop_rect;
	HWND desktop_window_handle = ::GetDesktopWindow();
	::GetClientRect(desktop_window_handle, &desktop_rect);
	const float32 desktop_width = static_cast<float32>(desktop_rect.right - desktop_rect.left);
	const float32 desktop_height = static_cast<float32>(desktop_rect.bottom - desktop_rect.top);
	const float32 desktop_aspect = desktop_width / desktop_height;

	// Calculate maximum client size (as some % of desktop size)
	float32 client_width = desktop_width * client_desktop_fraction;
	float client_height = desktop_height * client_desktop_fraction;
	if (aspect_ratio > desktop_aspect) {
		// Client window has a wider aspect than desktop; shrink client height to match its width
		client_height = client_width / aspect_ratio;
	} else {
		// Client window has a taller aspect than desktop; shrink client width to match its height
		client_width = client_height * aspect_ratio;
	}
	m_client_resolution = {static_cast<int32>(client_width), static_cast<int32>(client_height)};
	// Calculate client rect bounds by centering the client area
	const float32 client_margin_x = 0.5f * (desktop_width - client_width);
	const float32 client_margin_y = 0.5f * (desktop_height - client_height);
	RECT client_rect; {
		client_rect.left   = static_cast<int32>(client_margin_x);
		client_rect.top    = static_cast<int>(client_margin_y);
		client_rect.right  = client_rect.left + static_cast<int32>(client_width);
		client_rect.bottom = client_rect.top + static_cast<int>(client_height);
	}
	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT window_rect = client_rect;
	::AdjustWindowRectEx(&window_rect, window_style_flags, FALSE, window_style_ex);

	WCHAR window_title[1024];
	::MultiByteToWideChar(::GetACP(), 0, title.c_str(), -1, window_title, sizeof(window_title) / sizeof(window_title[0]));

	HWND hwnd = CreateWindowEx(
		window_style_ex,
		__GAME_WINDOW_CLASS_NAME,
		window_title,
		window_style_flags,
		window_rect.left,
		window_rect.top,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		nullptr,
		nullptr,
		::GetModuleHandle(nullptr),
		nullptr
	);

	ASSERT(hwnd != nullptr, "Failed to create window.");

	m_hwnd = static_cast<void*>(hwnd);
	game_window_proc = event_proc;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
	::ShowWindow(hwnd, SW_SHOW);
	::SetForegroundWindow(hwnd);
	::SetFocus(hwnd);

	HCURSOR cursor = LoadCursor(nullptr, IDC_ARROW);
	::SetCursor(cursor);

	// Start the game with 0 mouse movement; 
	m_last_mouse_position = get_client_mouse_position();
	m_current_mouse_position = get_client_mouse_position();
}

void window::destroy()
{
	if (!is_open()) {
		return;
	}
	::CloseWindow(static_cast<HWND>(m_hwnd));
	m_hwnd = nullptr;
}

void window::begin_frame()
{
	// 1. Dispatch messages
	MSG next_message;
	while (true) {
		const BOOL has_message = PeekMessage(&next_message, nullptr, 0, 0, PM_REMOVE);
		if (!has_message) {
			break;
		}
		::TranslateMessage(&next_message);
		DispatchMessage(&next_message);
	}

	// 2. Update Mouse
	m_last_mouse_position = m_current_mouse_position;
	m_current_mouse_position = get_client_mouse_position();
	if (m_mouse_input_mode == MOUSE_INPUT_MOVE_RELATIVE && is_focused()) {
		const ivec2 client_center = get_client_center();
		set_client_mouse_position(client_center);
		m_last_mouse_position = get_client_mouse_position();
	}
}

void window::end_frame()
{
	// Nothing now
}

bool window::is_focused() const
{
	return (static_cast<void*>(::GetActiveWindow()) == m_hwnd);
}

ivec2 window::get_client_resolution() const
{
	return m_client_resolution;
}

ivec2 window::get_client_center() const
{
	RECT client_rect;
	ivec2 center;
	::GetClientRect(static_cast<HWND>(m_hwnd), &client_rect);
	center.x = (client_rect.left + client_rect.right) >> 1;
	center.y = (client_rect.bottom + client_rect.top) >> 1;
	return center;
}

bool window::is_mouse_locked() const
{
	return (m_mouse_lock_count > 0);
}

ivec2 window::get_client_mouse_position() const
{
	POINT mouse_position;
	::GetCursorPos(&mouse_position);
	::ScreenToClient(static_cast<HWND>(m_hwnd), &mouse_position);
	return { mouse_position.x, mouse_position.y };
}

ivec2 window::get_mouse_relative_movement() const
{
	return get_client_mouse_position() - m_last_mouse_position;
}

void window::set_mouse_input_mode(e_mouse_input_mode mode)
{
	m_mouse_input_mode = mode;
	if (m_mouse_input_mode == MOUSE_INPUT_MOVE_RELATIVE) {
		const ivec2 client_center = get_client_center();
		set_client_mouse_position(client_center);
		m_last_mouse_position = get_client_mouse_position();
		m_current_mouse_position = get_client_mouse_position();
	}
}

void window::set_client_mouse_position(const ivec2& position) const
{
	POINT mouse_position{ position.x , position.y };
	::ClientToScreen(static_cast<HWND>(m_hwnd), &mouse_position);
	::SetCursorPos(mouse_position.x, mouse_position.y);
}

void window::lock_mouse()
{
	++m_mouse_lock_count;
	if (m_mouse_lock_count > 0) {
		__lock_mouse_to_window(static_cast<HWND>(m_hwnd));
	}
	m_last_mouse_position = get_client_mouse_position();
	m_current_mouse_position = get_client_mouse_position();
}

void window::unlock_mouse()
{
	--m_mouse_lock_count;
	if (m_mouse_lock_count <= 0) {
		m_mouse_lock_count = 0;
		__unlock_mouse();
	}
}

void window::show_mouse() const
{
	::ShowCursor(TRUE);
}

void window::hide_mouse() const
{
	::ShowCursor(FALSE);
}
};
#endif