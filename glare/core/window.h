#pragma once
#include "glare/core/common.h"
#include "glare/math/vector.h"
namespace glare
{
enum e_mouse_input_mode : int32
{
	MOUSE_INPUT_MODE_ABSOLUTE,
	MOUSE_INPUT_MOVE_RELATIVE
};

using windows_event_callback_t = bool(void*, uint32 message, uint64 w_param, int64 l_param);
extern windows_event_callback_t* DEFAULT_WINDOW_EVENT_PROC;

class window
{
public:
	window();
	~window();

	// Resource
	NODISCARD bool is_open () const;
	void	create	(const string& title, float32 aspect_ratio, float32 client_desktop_fraction, windows_event_callback_t* event_proc);
	void	destroy	();

	// Update
	void	begin_frame	();
	void	end_frame	();

	// State
	NODISCARD bool	is_focused		() const;
	NODISCARD ivec2	get_client_resolution	() const;
	NODISCARD ivec2	get_client_center		() const;
	
	NODISCARD bool	is_mouse_locked	() const;
	NODISCARD ivec2	get_client_mouse_position	() const;
	NODISCARD ivec2	get_mouse_relative_movement	() const;

	void	set_mouse_input_mode		(e_mouse_input_mode mode);
	void	set_client_mouse_position	(const ivec2& position)	const;
	void	lock_mouse					();
	void	unlock_mouse				();
	void	show_mouse					()	const;
	void	hide_mouse					()	const;
	
public: //members
	windows_event_callback_t* game_window_proc;
	void* m_hwnd = nullptr;
	e_mouse_input_mode m_mouse_input_mode = MOUSE_INPUT_MODE_ABSOLUTE;
	int32 m_mouse_lock_count = 0;
	ivec2 m_client_resolution;
	ivec2 m_last_mouse_position;
	ivec2 m_current_mouse_position;

public:
	window(const window&)				= delete;
	window(const window&&)				= delete;
	window operator= (const window&)	= delete;
	window operator= (const window&&)	= delete;
};

};