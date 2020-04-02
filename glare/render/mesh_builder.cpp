#include "glare/render/mesh_builder.h"
#include "glare/core/assert.h"
#include "glare/math/aabb2.h"
#include "glare/math/obb2.h"
namespace glare
{
static vec2 g_default_box_uv[] = {
	{0, 0}, {1, 0},
	{0, 1}, {1, 1}
};
void mesh_builder::add_obb2(mesh& obj, const obb2& box, const vec2* uvs)
{
	const vec2* uv_array = uvs ? uvs : g_default_box_uv;
	vec2 corners[4]; {
		box.get_corners(corners);
	}
	super_vertex brush;
	brush.color = obj.m_builder_brush.color;
	index_t indices[4]; {
		brush.position = corners[0]; // implicit convert
		brush.uv = uv_array[0];
		indices[0] = add_vertex(obj, brush);
		brush.position = corners[1]; // implicit convert
		brush.uv = uv_array[1];
		indices[1] = add_vertex(obj, brush);
		brush.position = corners[2]; // implicit convert
		brush.uv = uv_array[2];
		indices[2] = add_vertex(obj, brush);
		brush.position = corners[3]; // implicit convert
		brush.uv = uv_array[3];
		indices[3] = add_vertex(obj, brush);
	}
	add_triangle_indices(obj, indices[0], indices[2], indices[1]);
	add_triangle_indices(obj, indices[1], indices[2], indices[3]);
}

void mesh_builder::add_aabb2(mesh& obj, const aabb2& box, const vec2* uvs)
{
	const vec2* uv_array = uvs ? uvs : g_default_box_uv;
	vec2 corners[4] = {
		{box.min.x, box.max.y}, //tl
		{box.max.x, box.max.y}, //tr
		{box.min.x, box.min.y}, //bl
		{box.max.x, box.min.y}, //br
	};
	super_vertex brush;
	brush.color = obj.m_builder_brush.color;
	index_t indices[4]; {
		brush.position = corners[0]; // implicit convert
		brush.uv = uv_array[0];
		indices[0] = add_vertex(obj, brush);
		brush.position = corners[1]; // implicit convert
		brush.uv = uv_array[1];
		indices[1] = add_vertex(obj, brush);
		brush.position = corners[2]; // implicit convert
		brush.uv = uv_array[2];
		indices[2] = add_vertex(obj, brush);
		brush.position = corners[3]; // implicit convert
		brush.uv = uv_array[3];
		indices[3] = add_vertex(obj, brush);
	}
	add_triangle_indices(obj, indices[0], indices[2], indices[1]);
	add_triangle_indices(obj, indices[1], indices[2], indices[3]);
}

void mesh_builder::add_line2(mesh& obj, const vec2& start, const vec2& end, float thick)
{
	UNIMPLEMENTED_BREAK;
}

void mesh_builder::add_disk(mesh& obj, const vec2& center, float radius, uint32 slice, const aabb2& uv_box)
{
	index_t* indices = new index_t[slice + 2u];
	super_vertex brush;
	const vec2 size = uv_box.get_size();
	brush.color = obj.m_builder_brush.color;
	brush.uv = uv_box.get_center();
	brush.position = center;
	indices[0] = add_vertex(obj, brush);
	for(size_t i = 1; i <= slice; ++i) {
		const float32 theta = static_cast<float32>(i - 1) / static_cast<float32>(slice) * PI * 2.f;
		vec2 uv {cosf(theta), sinf(theta)};
		//uv.y = 1.f - uv.y;
		uv *= 0.5f;
		uv += {0.5f, 0.5f};
		uv *= size;
		uv += uv_box.min;
		brush.uv = uv;
		vec2 position {cosf(theta), sinf(theta)};
		position *= radius;
		position += center;
		brush.position = position;
		indices[i] = add_vertex(obj, brush);
	}
	indices[slice + 1] = indices[1];
	for (size_t i = 1; i <= slice; ++i) {
		add_triangle_indices(obj, indices[0], indices[i], indices[i + 1]);
	}
	delete[] indices;
}

void mesh_builder::add_ring(mesh& obj, const vec2& center, float radius, float thick, uint32 slice)
{
	UNIMPLEMENTED_BREAK;
}

void mesh_builder::add_grid2(mesh& obj, const vec2& bl, const vec2& tr, uint32 x_step, uint32 y_step)
{
	UNIMPLEMENTED_BREAK;
}

mesh_builder::index_t mesh_builder::add_vertex(mesh& obj, const super_vertex& vertex)
{
	auto& verts = obj.m_verts;
	verts.emplace_back(vertex);
	obj.m_vertex_dirty = true;
	return static_cast<index_t>(verts.size() - 1);
}

void mesh_builder::add_triangle_indices(mesh& obj, index_t i0, index_t i1, index_t i2)
{
	auto& indices = obj.m_indices;
#if defined(GLARE_DEBUG)
	const size_t size = obj.m_verts.size();
	if (i0 >= size || i1 >= size || i2 >= size) {
		ALERT("Index out of bound while building meshes.");
	}
#endif
	indices.emplace_back(i0);
	indices.emplace_back(i1);
	indices.emplace_back(i2);
	obj.m_indices_dirty = true;
}
}
