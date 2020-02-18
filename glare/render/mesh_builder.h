/// Auto build
/// Use glare::mesh::m_builder_brush.m_color only
///
/// Boxes auto build
/// for all boxes building, the uv array is in "Z" order, i.e.
/// [0]tl->[1]tr
///	      /
///	     /
///	    /
///	[2]bl->[3]br
/// by default {{0,0}, {1,0}, {0,1}, {1,1}} is used

#pragma once
#include "glare/render/mesh.h"
#include "glare/render/vertex.h"
#include "glare/math/aabb2.h"

namespace glare
{
struct aabb2;
struct obb2;

namespace mesh_builder
{
	using index_t = index_buffer::index_t;
	// Check "Boxes auto build" for more info
	void add_obb2(mesh& obj, const obb2& box, const vec2* uvs=nullptr);
	// Only for aabb2 in right-hand world Cartesian space: x grows up, y grows right
	void add_aabb2(mesh& obj, const aabb2& box, const vec2* uvs=nullptr);
	void add_line2(mesh& obj, const vec2& start, const vec2& end, float thick);
	void add_disk(mesh& obj, const vec2& center, float radius, uint32 slice=32, const aabb2& uv_box=aabb2::UNIT);
	void add_ring(mesh& obj, const vec2& center, float radius, float thick, uint32 slice=32);
	void add_grid2(mesh& obj, const vec2& bl, const vec2& tr, uint32 x_step=2, uint32 y_step=2);

	// Add a vertex and set this point to mesh's build brush
	// Return: the index
	index_t add_vertex(mesh& obj, const super_vertex& vertex);
	void add_triangle_indices(mesh& obj, index_t i0, index_t i1, index_t i2);
}
}