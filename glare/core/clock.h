#pragma once
#include "glare/core/common.h"
#include "glare/core/event.h"
#include <vector>
namespace glare
{
class clock;
extern clock* g_master_clock;

float64 get_current_time_seconds();

class clock
{
public:
	clock	();
	clock	(clock* parent = g_master_clock);
	clock	(const clock&) = delete;
	clock	(clock&&) = delete;
	clock&	operator=(const clock&) = delete;
	clock&	operator=(clock&&) = delete;
	virtual ~clock	() noexcept;

	void	set_parent		(clock* parent);
	virtual void	add_child		(clock* child);
	virtual void	remove_child	(clock* child);
	void	pause			();
	void	unpause			();
	virtual void	step	(float64 delta_seconds);

	void	set_fps			(const float fps);
public:
	clock*				m_parent		= nullptr;
	float32				m_scale		= 1.f;
	float32				m_frame_time	= 0.f;
	float32				m_frame_limit	= 1.f / 60.f;
	std::vector<clock*>	m_children;
	bool				m_paused		= false;
	bool				m_garbage		= false;	//Never set to true if a clock have a child

public:
	void	set_scale(const float scale)				{ m_scale = scale; }
	void	set_frame_limit(const float frame_limit)	{ m_frame_limit = frame_limit; }
	NODISCARD float32	get_scale() const				{ return m_scale; }
	NODISCARD float32	get_frame_limit() const			{ return m_frame_limit; }
	NODISCARD float32	get_frame_time() const			{ return m_frame_time; }
	NODISCARD bool		is_paused() const				{ return m_paused; }
};

class countdown_clock : public clock
{
public:
	countdown_clock(float32 countdown_seconds, string event_id, const dict& args, clock* parent = g_master_clock);
	void step	(float64 delta_seconds) override final;
	void add_child(clock* child) override final;
	void remove_child(clock* child) override final;
public:
	float32	m_countdown_seconds;
	string	m_event_id;
	dict	m_args;
};

}