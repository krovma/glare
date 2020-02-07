#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"

namespace glare
{
class renderer;
class buffer_layout;
class shader_stage
{
public:
	~shader_stage();
	enum e_shader_stage
	{
		VERTEX_SHADER = 0,
		PIXEL_SHADER,

		NUM_SHADER_STAGES
	};
	static const char* get_shader_model(e_shader_stage stage);

	void compile(const renderer* r, const string& src, e_shader_stage stage, const char* filename, const char* entry_point);
	NODISCARD bool is_valid() const { return m_handle != nullptr; }
	NODISCARD dx_bytecode* get_bytecode() const {return m_bytecode; }
public:
	union
	{
		dx_resource*	m_handle;
		dx_vs*			m_vertex_shader;
		dx_ps*			m_pixel_shader;
	};
	dx_bytecode* m_bytecode = nullptr;
	e_shader_stage m_stage = VERTEX_SHADER;
};

class shader
{
public:
	static shader* load_from_xml(const char* path, renderer* r);
public:
	shader(renderer* r);
	~shader();

	bool load_hlsl(const char* path, const char* vertex_entry, const char* pixel_entry);
	// NODISCARD dx_vs*	get_vertex_shader()	const;
	// NODISCARD dx_ps*	get_pixel_shader()	const;
	// NODISCARD dx_layout* get_vbo_layout()	const;
	NODISCARD bool		is_valid()			const;
	NODISCARD dx_vs* get_dx_vs() const {return m_vs.m_vertex_shader;}
	NODISCARD dx_ps* get_dx_ps() const {return m_ps.m_pixel_shader;}
	
	void create_dx_vbo_layout(const buffer_layout* layout);
	void set_blend_mode(e_blend_mode blend_mode);
	void update_blend_mode();
	void set_depth_stencil_mode(e_compare_operator pass_op, bool write);
	void update_depth_stencil_mode();
	void set_rasterizer_mode(e_cull_mode cull_mode, e_fill_mode fill_mode, bool front_face_ccw=true, bool depth_clip=true);
	void update_rasterizer_mode();

	void update_all_mode();
	void reset_shader_mode();
	void use_state(dx_state_blend* blend, dx_state_depth_stencil* depth_stencil, dx_state_rasterizer* rasterizer);
public:
	renderer*			m_renderer		= nullptr;
	dx_layout*			m_vbo_layout	= nullptr;
	dx_state_blend*			m_dx_blend_state			= nullptr;
	dx_state_rasterizer*	m_dx_rasterizer_state		= nullptr;
	dx_state_depth_stencil*	m_dx_depth_stencil_state	= nullptr;
	shader_stage		m_vs;
	shader_stage		m_ps;

	// accessible state value
	e_blend_mode		m_blend_mode	= BLEND_ALPHA;
	e_compare_operator	m_depth_comp_op	= COMP_GREATER;
	bool				m_write_depth	= true;
	e_cull_mode			m_cull_mode		= CULL_BACK;
	e_fill_mode			m_fill_mode		= FILL_SOLID;
	bool				m_front_ccw		= true;
	bool				m_depth_clip	= true;

	// update flag
	bool m_update_blend_mode = true;
	bool m_update_rasterizer = true;
	bool m_update_depth_stencil = true;
};
};