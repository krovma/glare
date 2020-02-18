#include "glare/render/surface.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
namespace glare
{
surface* surface::load_image(const string& id, const char* path, bool flip_v)
{
	const auto found = s_cached.find(id);
	if (found == std::end(s_cached)) {
		surface* created = new surface(path, flip_v);;
		s_cached[id] = created;
		return created;
	}
	return found->second;
}

surface::surface(const char* path, bool flip_v)
{
	::stbi_set_flip_vertically_on_load(flip_v ? 1 : 0);
	int num_channels;
	m_raw = stbi_load(path, &m_size.x, &m_size.y, &num_channels, 0);
	m_raw_channels = static_cast<uint8>(num_channels);
	const size_t num_texels = static_cast<size_t>(m_size.x) * static_cast<size_t>(m_size.y);
	m_surface_data.reserve(num_texels);
	for (size_t texel_idx = 0; texel_idx < num_texels; ++texel_idx) {
		const size_t offset = texel_idx * m_raw_channels;
		byte* const raw_color = m_raw + offset;
		rgba8 texel8(raw_color[0], raw_color[1], raw_color[2]);
		if (m_raw_channels == 4) {
			texel8.a = m_raw[offset + 3];
		}
		m_surface_data.emplace_back(rgba(texel8));
	}
	m_path = path;
}

surface::surface(size_t w, size_t h, const rgba& default_color)
	:m_size(static_cast<int32>(w), static_cast<int32>(h))
{
	m_surface_data.resize(w * h, default_color);
}

surface::surface(size_t w, size_t h, const rgba* texel_data)
	:m_size(static_cast<int32>(w), static_cast<int32>(h))
{
	m_surface_data.resize(w * h);
	memcpy(m_surface_data.data(), texel_data, w * h * sizeof(rgba));
}

surface::~surface()
{
	if (!m_path.empty() && m_raw) {
		::stbi_image_free(m_raw);
	} else {
		delete[] m_raw;
	}
}

const rgba surface::get_texel(const ivec2& texel_coord) const
{
	const size_t texel_index = static_cast<size_t>(texel_coord.v) * m_size.u + texel_coord.u;
	return m_surface_data[texel_index];
}

void surface::set_texel(const ivec2& texel_coord, const rgba& color)
{
	const size_t texel_index = static_cast<size_t>(texel_coord.v) * m_size.u + texel_coord.u;
	m_surface_data[texel_index] = color;
	if (m_raw) {
		const rgba8 color8(color);
		const size_t offset = texel_index * m_raw_channels * sizeof(byte);
		byte* const raw_color = m_raw + offset;
		raw_color[0] = color8.r;
		raw_color[1] = color8.g;
		raw_color[2] = color8.b;
		if (m_raw_channels == 4) {
			raw_color[3] = color8.a;
		}
	}
}

rgba* surface::get_surface_buffer()
{
	return m_surface_data.data();
}

const rgba* surface::get_surface_buffer() const
{
	return m_surface_data.data();
}

void surface::write_png(const char* path) const
{
	if (!m_raw) {
		const size_t num_texel = static_cast<size_t>(m_size.u) * m_size.v;
		m_raw = new byte[num_texel * m_raw_channels];
		for (size_t texel_idx = 0; texel_idx < num_texel; ++texel_idx) {
			const rgba8 color8(m_surface_data[texel_idx]);
			const size_t offset = m_raw_channels * texel_idx * sizeof(byte);
			byte* const raw_color = m_raw + offset;
			raw_color[0] = color8.r;
			raw_color[1] = color8.g;
			raw_color[2] = color8.b;
			if (m_raw_channels == 4) {
				raw_color[3] = color8.a;
			}
		}
	}
	::stbi_write_png(path, m_size.u, m_size.v, 4, m_raw, static_cast<int>(m_raw_channels * m_size.x));
}

STATIC std::unordered_map<string, surface*> surface::s_cached;
}
