#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"
#include "glare/math/vector.h"

namespace glare
{
class renderer;
class surface;
class texture
{
public:
	texture(renderer* r)
		: m_renderer(r)
	{
	}
	virtual ~texture()
	{
		DX_RELEASE(m_srv);
		DX_RELEASE(m_handle);
	}

	NODISCARD string		get_texture_name() const { return m_name; }
	NODISCARD virtual dx_resource*	get_texture_handle() const { return m_handle; }
	NODISCARD bool		is_immutable() const {return m_memory_usage == GPU_MEMORY_IMMUTABLE; }

	void set_texture_name(const string& name) { m_name = name; }

	virtual void create_view() const = 0;
	NODISCARD virtual dx_srv* get_view_handle() const = 0;

public:
	renderer*		m_renderer = nullptr;
	dx_resource*	m_handle = nullptr;
	mutable dx_srv*	m_srv = nullptr;
	string			m_name;

	e_gpu_memory_usage	m_memory_usage = GPU_MEMORY_IMMUTABLE;
	int32				m_texture_usage = TEXTURE_SHADER_RESOURCE;
};

class texture2d : public texture
{
public:
	texture2d(renderer* r)
		: texture(r)
	{
	}
	texture2d(renderer*r, dx_texture2d* ref_dx_texture);
	texture2d(renderer*r, const surface* from_surface);
	~texture2d() override
	{
		//texture::~texture();
	}
	void wrap_dx_texture(dx_texture2d* to_wrap);
	
	NODISCARD virtual dx_texture2d* get_texture_handle() const override
	{
		return static_cast<dx_texture2d*>(m_handle);
	}
	NODISCARD virtual dx_srv* get_view_handle() const override;
	virtual void create_view() const override;

	NODISCARD ivec2 get_size() const { return m_size; }
	
	ivec2 m_size;
};

};
