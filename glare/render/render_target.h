#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"
#include "glare/math/vector.h"

namespace glare
{
class render_target
{
public:
	render_target() = default;
	~render_target()
	{
		DX_RELEASE(m_rtv);
	}

	NODISCARD int32 get_height() const			{ return m_size.y; }
	NODISCARD int32 get_width() const			{ return m_size.x; }
	NODISCARD float32 get_aspect_ratio() const	{ return static_cast<float32>(m_size.x) / static_cast<float32>(m_size.y); }
	NODISCARD dx_rtv* get_dx_handle() const		{ return m_rtv; }

	void make_from_dx_texture(dx_device* device, dx_texture2d* texture);

public:
	dx_rtv*	m_rtv = nullptr;
	ivec2 m_size;
};

};