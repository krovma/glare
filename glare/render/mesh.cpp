#include "glare/render/mesh.h"
#include "core/assert.h"

namespace glare
{
void mesh::clear()
{
	m_verts.clear();
	m_indices.clear();
	m_builder_brush = super_vertex();
}

void mesh::update_gpu_mesh(renderer* r)
{
	if (m_vertex_dirty) {
		ASSERT(m_layout, "Buffer layout must be assigned.");
		delete m_gpu_vbo;
		m_gpu_vbo = new vertex_buffer(r);
		const size_t vertex_count = get_vertex_count();
		const size_t vbo_size = m_layout->get_stride() * vertex_count;
		byte* buffer = new byte[vbo_size];
		m_layout->copy_from_super_vertex(buffer, m_verts.data(), vertex_count);
		m_gpu_vbo->create_immutable_buffer(buffer, vertex_count, m_layout);
		delete[] buffer;
		m_vertex_dirty = false;
	}
	if (m_using_ibo && m_indices_dirty) {
		delete m_gpu_ibo;
		m_gpu_ibo = new index_buffer(r);
		m_gpu_ibo->create_immutable_buffer(m_indices.data(), m_indices.size());
	}
}
}
