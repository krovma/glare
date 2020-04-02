#include "glare/core/clock.h"
#include "glare/math/utilities.h"
#include "glare/core/assert.h"
#include <Windows.h>
namespace glare
{
///<summary>
///Global master clock, this clock is created before <c>app.start()</c>
///</summary>
///<remarks>
///In main_[platform].cpp, g_master_clock should be instantiated with new clock(nullptr)
///</remarks>
clock* g_master_clock = nullptr;
//////////////////////////////////////////////////////////////////////////
////////////////////////////////
clock::clock()
	: m_parent(g_master_clock)
{
	m_parent->add_child(this);
}

////////////////////////////////
clock::clock(clock* parent)
	: m_parent(parent)
{
	if (parent) {
		parent->add_child(this);
	}
}

////////////////////////////////
clock::~clock() noexcept
{
	if (!m_parent) {
		m_parent = g_master_clock;
	}
	try {
		for (auto& each_child : m_children) {
			each_child->set_parent(m_parent);
		}
	} catch(std::exception& e) {
		FATAL("Exception catched: %s", e.what());
	}
}

////////////////////////////////
void clock::set_parent(clock* parent)
{
	parent->add_child(this);
	m_parent = parent;
}

////////////////////////////////
void clock::add_child(clock* child)
{
	for (auto& each_child : m_children) {
		if (each_child == child) {
			return;
		}
	}
	m_children.push_back(child);
}

////////////////////////////////
void clock::remove_child(clock* child)
{
	for (auto iter = std::begin(m_children); iter != std::end(m_children); ++iter) {
		if (*iter == child) {
			m_children.erase(iter);
			return;
		}
	}
}

////////////////////////////////
void clock::step(float64 delta_seconds)
{
	if (m_paused || m_garbage) {
		m_frame_time = 0.f;
	} else {
		m_frame_time = clamp(static_cast<float>(delta_seconds) * m_scale, 0.f, m_frame_limit);
	}
	for (auto& each : m_children) {
		each->step(static_cast<float64>(m_frame_time));
	}

	for (auto it = std::begin(m_children); it != std::end(m_children); ) {
		if ((*it)->m_garbage) {
			delete *it;
			it = m_children.erase(it);
		} else {
			++it;
		}
	}
}

////////////////////////////////
void clock::pause()
{
	m_paused = true;
}

////////////////////////////////
void clock::unpause()
{
	m_paused = false;
}

////////////////////////////////
void clock::set_fps(const float fps)
{
	m_frame_limit = 1.0f / fps;
}

countdown_clock::countdown_clock(float32 countdown_seconds, string event_id, const dict& args, clock* parent)
	: clock(parent)
	, m_countdown_seconds(countdown_seconds)
	, m_event_id(std::move(event_id))
	, m_args(args)
{
}

void countdown_clock::step(float64 delta_seconds)
{
	if (m_paused || m_garbage) {
		return;
	}
	m_countdown_seconds -= static_cast<float32>(delta_seconds);
	if (m_countdown_seconds <= 0.f) {
		event::fire(m_event_id, m_args);
		m_garbage = true;
	}
}

void countdown_clock::add_child(clock* child)
{
	ALERT("Adding child to countdown_clock is not supported");
}

void countdown_clock::remove_child(clock* child)
{
	ALERT("Removing child to countdown_clock is not supported");
}

float64 init_time( LARGE_INTEGER& out_initial_time )
{
	LARGE_INTEGER counts_per_second;
	QueryPerformanceFrequency( &counts_per_second );
	QueryPerformanceCounter( &out_initial_time );
	return( 1.0 / static_cast<float64>( counts_per_second.QuadPart ) );
}

static LARGE_INTEGER initial_time;
static float64 sec_per_count = init_time(initial_time);

//-----------------------------------------------------------------------------------------------
float64 get_current_time_seconds()
{
	LARGE_INTEGER current_count;
	QueryPerformanceCounter( &current_count );
	const LONGLONG elapsed_counts_since_initial_time = current_count.QuadPart - initial_time.QuadPart;
	const float64 current_seconds = static_cast<float64>( elapsed_counts_since_initial_time ) * sec_per_count;
	return current_seconds;
}

}
