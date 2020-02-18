#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"
#include "glare/render/buffer.h"
#include "glare/render/texture.h"
#include "glare/render/render_target.h"
#include "glare/math/vector.h"
#include "glare/math/matrix.h"
#include <unordered_map>

namespace glare
{
class mesh;
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

	// Resource
	texture2d*	load_texture2d_from_file(const string& id, const char* path, bool flip_v=false);
	NODISCARD texture2d*	get_texture2d_by_id(const string& id) const;
	
	// Render states
	void set_ortho(const vec2& ortho_min, const vec2& ortho_max, float32 near_z, float32 far_z);
	void set_viewport(const ivec2& size, const ivec2& topleft) const;
	void reset_viewport() const;
	void copy_texture(texture* dst, texture* src) const;
	void bind_shader(shader* shader);
	void bind_vbo(vertex_buffer* vbo) const;
	void bind_ibo(index_buffer* ibo) const;
	void bind_constant_buffer(e_constant_buffer_id buffer_id, constant_buffer* buffer) const;
	void bind_texture(const texture* tex, e_texture_slot slot=TEXTURE_SLOT_DIFFUSE, e_texture_filter filter=MIN_POINT_MAG_POINT) const;
	// Render operations
	void clear_render_target(const rgba& clear_color) const;
	void draw(size_t vertex_count, size_t offset=0) const;
	void draw_indexed(size_t indice_count, size_t offset=0) const;
	void draw_mesh(const mesh* mesh) const;
public: //members
	ivec2			m_resolution;
	mat4			m_projection = mat4::identity;
	dx_device*		m_device = nullptr;
	dx_context*		m_context = nullptr;
	dx_swapchain*	m_swapchain = nullptr;
	dx_debug*		m_debug = nullptr;

	vertex_buffer*		m_buffer_vbo = nullptr;
	constant_buffer*	m_buffer_project = nullptr;
	constant_buffer*	m_buffer_model = nullptr;
	constant_buffer*	m_buffer_post = nullptr;
	shader*				m_current_shader = nullptr;

	texture2d*		m_frame_back_buffer_texture = nullptr;
	texture2d*		m_frame_texture = nullptr;
	render_target*	m_frame_render_target = nullptr;

public: // static member
	static std::unordered_map<string, texture2d*> s_cached_texture;
public: //delete
	renderer(const renderer&) = delete;
	renderer(const renderer&&) = delete;
	renderer& operator=(const renderer&) = delete;
	renderer& operator=(const renderer&&) = delete;
};
};