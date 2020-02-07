#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"
#include "glare/render/buffer.h"
#include "glare/render/texture.h"
#include "glare/render/render_target.h"
#include "glare/math/vector.h"

namespace glare
{
struct rgba;
class window;
class shader;
class renderer
{
public:
	renderer(const window* client);
	~renderer() = default;

	void start();
	void begin_frame();
	void end_frame();
	void stop();
	
	NODISCARD dx_device* get_dx_device() const { return m_device; }
	NODISCARD dx_context* get_dx_context() const { return m_context; }
	
	void copy_texture(texture* dst, texture* src) const;
	void bind_shader(shader* shader);
	void bind_vbo(vertex_buffer* vbo) const;
	// Render operations
	void clear_render_target(const rgba& clear_color) const;
	void draw(size_t vertex_count, size_t offset=0) const;
public: //members
	ivec2			m_resolution;
	dx_device*		m_device = nullptr;
	dx_context*		m_context = nullptr;
	dx_swapchain*	m_swapchain = nullptr;
	dx_debug*		m_debug = nullptr;

	vertex_buffer*		m_buffer_vbo = nullptr;
	constant_buffer*	m_buffer_model = nullptr;
	constant_buffer*	m_buffer_post = nullptr;
	shader*				m_current_shader = nullptr;

	texture2d*		m_frame_back_buffer_texture = nullptr;
	texture2d*		m_frame_texture = nullptr;
	render_target*	m_frame_render_target = nullptr;
	
public: //delete
	renderer(const renderer&) = delete;
	renderer(const renderer&&) = delete;
	renderer& operator=(const renderer&) = delete;
	renderer& operator=(const renderer&&) = delete;
};
};