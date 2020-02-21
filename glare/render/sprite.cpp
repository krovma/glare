#include "glare/render/sprite.h"
#include "glare/render/renderer.h"
#include "core/assert.h"
#include "core/string_utils.h"

namespace glare
{
i_sprite_anim_base::~i_sprite_anim_base()
{
	// Definition of pure virtual destructor
}

void sprite::get_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const
{
	bottom_left = m_bottom_left;
	top_right = m_top_right;
	bottom_right = {m_top_right.u, m_bottom_left.v};
	top_left = {m_bottom_left.u, m_top_right.v};
}

const texture2d* sprite::get_texture2d() const
{
	return m_sprite_sheet->m_texture;
}

sprite_sheet::sprite_sheet(texture2d* texture, const ivec2& layout)
	:m_texture(texture), m_layout(layout)
{
	const vec2 stride {1.f / static_cast<float>(layout.u), 1.f/ static_cast<float>(layout.v)};
	const size_t n_sprites = layout.u * layout.v;
	for (size_t i = 0; i < n_sprites; ++i) {
		const ivec2 coord(i % layout.u, i / layout.v);
		vec2 uv_bl = stride * vec2(coord.x, coord.y + 1);
		vec2 uv_tr {uv_bl.u + stride.u, uv_bl.v - stride.v};
		m_sprites.emplace_back(sprite(this, uv_bl, uv_tr));
	}
}

sprite_sheet::sprite_sheet(renderer* r, xml::document* xml_doc)
{
	xml::node root_node = xml_doc->root();
	xml::attribute id = root_node.attribute("id");
	m_texture = r->get_texture2d_by_id(id.value());
	if (!m_texture) {
		// texture not pre-loaded
		// try load from .src
		xml::attribute src = root_node.attribute("src");
		if (src.empty()) {
			FATAL("No available texture nor image file when loading sprite sheet");
		}
		m_texture = r->load_texture2d_from_file(id.value(), src.value());
	}
	vec2 inv_texture_size(m_texture->get_size());
	inv_texture_size = {1.f / inv_texture_size.u, 1.f / inv_texture_size.v};
	auto sprites = root_node.children("sprite");
	const size_t sprite_count = std::distance(std::begin(sprites), std::end(sprites));
	m_sprites.resize(sprite_count);
	for(auto& each : sprites) {
		const size_t index = each.attribute("index").as_uint(UINT_MAX); // try to fail
		vec2 bottom_left = xml::get_attr(each, "bl", vec2::ZERO);
		vec2 top_right = xml::get_attr(each, "tr", vec2::ZERO);
		bottom_left *= inv_texture_size;
		top_right *= inv_texture_size;
		auto& current_sprite = m_sprites[index];
		current_sprite.m_sprite_sheet = this;
		current_sprite.m_bottom_left = bottom_left;
		current_sprite.m_top_right = top_right;
	}
}

}
