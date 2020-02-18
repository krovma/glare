#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"
#include "glare/render/buffer.h"
#include "glare/render/vertex.h"
namespace glare
{
class mesh
{
public:
	using index_t = index_buffer::index_t;
public:
	mesh(const buffer_layout* layout) : m_layout(layout) {}
	mesh(const mesh& copy_from)
		: m_verts(copy_from.m_verts)
		, m_indices(copy_from.m_indices)
		, m_builder_brush(copy_from.m_builder_brush)
		, m_layout(copy_from.m_layout)
		, m_using_ibo(copy_from.m_using_ibo)
	{
	}
	~mesh()
	{
		delete m_gpu_vbo;
		delete m_gpu_ibo;
	}
	void clear();
	NODISCARD size_t get_vertex_count()	 const { return m_verts.size(); }
	NODISCARD size_t get_indices_count() const { return m_indices.size(); }
	NODISCARD bool	 is_indexed()		 const { return m_using_ibo; }
	NODISCARD size_t get_element_count() const { return is_indexed() ? get_indices_count() : get_vertex_count(); }

	void update_gpu_mesh(renderer* r);
public:
	// CPU
	std::vector<super_vertex>	m_verts;
	std::vector<index_t>		m_indices;
	// used by mesh builder
	super_vertex				m_builder_brush;
	// GPU
	const buffer_layout*	m_layout = nullptr;
	vertex_buffer*	m_gpu_vbo = nullptr;
	index_buffer*	m_gpu_ibo = nullptr;
	bool			m_using_ibo = true;
	bool			m_vertex_dirty = true;
	bool			m_indices_dirty = true;
};


}
