#include "glare/render/sprite.h"
#include "glare/render/renderer.h"
#include "core/assert.h"
#include "core/string_utils.h"
#include <algorithm>

namespace glare
{
STATIC std::unordered_map<string, sprite_sheet*> sprite_sheet::s_sprite_sheet_cache;

STATIC std::unordered_map<string, sprite_anim*> sprite_anim::s_sprite_anim_cache;

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

void sprite::get_current_frame_uvs(vec2 uvs[4]) const
{
	get_uvs(uvs[0], uvs[1], uvs[2], uvs[3]);
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
		const ivec2 coord(i % layout.u, i / layout.u);
		vec2 uv_bl = stride * vec2(coord.x, coord.y + 1);
		vec2 uv_tr {uv_bl.u + stride.u, uv_bl.v - stride.v};
		m_sprites.emplace_back(sprite(this, uv_bl, uv_tr));
	}
}

sprite_sheet::sprite_sheet(renderer* r, xml::document* xml_doc)
{
	xml::node root_node = xml_doc->root().child("sprites");
	//xml::attribute id = root_node.attribute("id");
	xml::attribute texture_id = root_node.attribute("texture");
	m_texture = r->get_texture2d_by_id(texture_id.value());
	if (!m_texture) {
		// texture not pre-loaded
		// try load from .src
		xml::attribute src = root_node.attribute("src");
		if (src.empty()) {
			FATAL("No available texture nor image file when loading sprite sheet");
		}
		m_texture = r->load_texture2d_from_file(texture_id.value(), src.value());
	}
	vec2 inv_texture_size(m_texture->get_size());
	inv_texture_size = {1.f / inv_texture_size.u, 1.f / inv_texture_size.v};
	auto sprites = root_node.children("sprite");
	const size_t sprite_count = std::distance(std::begin(sprites), std::end(sprites));
	m_sprites.resize(sprite_count);
	for(auto& each : sprites) {
		const size_t index = xml::get_attr(each, "index", UINT_MAX); // try to fail
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

void sprite_sheet::load_sprite_sheet_from_texture(const string& id, const ivec2& layout, texture2d* texture)
{
	auto found = s_sprite_sheet_cache.find(id);
	if (found != std::end(s_sprite_sheet_cache)) {
		delete found->second;
	}
	sprite_sheet* created = new sprite_sheet(texture, layout);
	s_sprite_sheet_cache[id] = created;
}

void sprite_sheet::load_sprite_sheet_from_xml(const string& id, renderer* r, const char* path)
{
	auto found = s_sprite_sheet_cache.find(id);
	if (found != std::end(s_sprite_sheet_cache)) {
		delete found->second;
	}
	xml::document* doc = xml::load_file(path);
	sprite_sheet* created = new sprite_sheet(r, doc);
	xml::unload_file(doc);
	s_sprite_sheet_cache[id] = created;
}

sprite_sheet* sprite_sheet::get_sprite_sheet(const string& id)
{
	const auto found = s_sprite_sheet_cache.find(id);
	ASSERT(found != std::end(s_sprite_sheet_cache), "Sprite sheet with given ID not found.");
	return found->second;
}

void sprite_sheet::clear_cache()
{
	for(auto& each : s_sprite_sheet_cache) {
		delete each.second;
	}
	s_sprite_sheet_cache.clear();
}

sprite_anim_clip::sprite_anim_clip(std::vector<frame> frames, bool start_paused)
	: m_total_frame(frames.size())
	, m_paused(start_paused)
	, m_frames(std::move(frames))
{
	m_current_frame_rest_time = m_frames[0].m_frame_time;
}

void sprite_anim_clip::get_current_frame_uvs(vec2 uvs[4]) const
{
	m_frames[m_current_frame].m_sprite.get_current_frame_uvs(uvs);
}

void sprite_anim_clip::get_current_frame_uvs(
	vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const
{
	m_frames[m_current_frame].m_sprite.get_current_frame_uvs(top_left, top_right, bottom_left, bottom_right);
}

void sprite_anim_clip::advance_time(float32 delta_sec)
{
	if (m_paused) {
		return;
	}
	while(!m_paused && delta_sec >= m_current_frame_rest_time) {
		delta_sec -= m_current_frame_rest_time;
		// what is next frame?
		m_current_frame += m_forward ? 1 : -1;
		if (m_playback_mode == e_playback_mode::PLAYBACK_ONCE) {
			if (m_current_frame >= m_total_frame) {
				m_paused = true;
				m_current_frame = m_total_frame - 1;
			} else if (m_current_frame < 0) {
				m_paused = true;
				m_current_frame = 0;	
			}
		} else if (m_playback_mode == e_playback_mode::PLAYBACK_LOOP) {
			if (m_current_frame >= m_total_frame) {
				m_current_frame = 0;
			} else if (m_current_frame < 0) {
				m_current_frame = m_total_frame - 1;	
			}
		} else if (m_playback_mode == e_playback_mode::PLAYBACK_PINGPONG) {
			if (m_current_frame >= m_total_frame) {
				m_current_frame = std::max(m_total_frame - 2, 0);
				m_forward = false;
			} else if (m_current_frame < 0) {
				m_current_frame = std::min(1, m_total_frame - 1);
				m_forward = true;
			}
		}
		m_current_frame_rest_time = m_frames[m_current_frame].m_frame_time;
	}
	ASSERT(m_current_frame_rest_time > delta_sec, "Check implementation");
	m_current_frame_rest_time -= delta_sec;
}

void sprite_anim_clip::pause()
{
	m_paused = true;
}

void sprite_anim_clip::unpause()
{
	m_paused = false;
}

void sprite_anim_clip::reset()
{
	m_current_frame = 0;
	m_current_frame_rest_time = m_frames[0].m_frame_time;
	m_paused = false;
	m_forward = true;
}

sprite_anim::sprite_anim(const xml::node& xml)
{
	const string sprite_sheet_id = xml::get_attr(xml, "sheet", "default");
	m_sprite_sheet = sprite_sheet::get_sprite_sheet(sprite_sheet_id);
	for (auto& each_clip: xml.children("clip")) {
		const string clip_id = xml::get_attr(each_clip, "id", "no-clip-id");
		if (m_current_clip_id == "UNSET") {
			m_current_clip_id = clip_id;
		}
		const string mode = xml::get_attr(each_clip, "mode", "loop");
		const bool paused = xml::get_attr(each_clip, "start_paused", false);
		std::vector<sprite_anim_clip::frame> frames;
		auto xml_frames = each_clip.children("frame");

		for(auto& each_frame: xml_frames) {
			const int32 index = xml::get_attr(each_frame, "sprite_index", 0);
			float32 time = xml::get_attr(each_frame, "time", 0.f);
			sprite* frame_sprite_copy = m_sprite_sheet->create_sprite_copy(index);
			frames.emplace_back(*frame_sprite_copy, time);
			delete frame_sprite_copy;
		}
		sprite_anim_clip new_clip(frames, paused);
		if (mode == "once") {
			new_clip.m_playback_mode = sprite_anim_clip::e_playback_mode::PLAYBACK_ONCE;
		} else if (mode == "pingpong") {
			new_clip.m_playback_mode = sprite_anim_clip::e_playback_mode::PLAYBACK_PINGPONG;
		} else {
			new_clip.m_playback_mode = sprite_anim_clip::e_playback_mode::PLAYBACK_LOOP;
		}
		m_clips.emplace(clip_id, new_clip);
	}
}

sprite_anim::sprite_anim(const sprite_anim& copy_from)
	: m_sprite_sheet(copy_from.m_sprite_sheet)
	, m_current_clip_id(copy_from.m_current_clip_id)
	, m_clips(copy_from.m_clips)
{
}

sprite_anim::sprite_anim(sprite_anim&& moved) noexcept
	: m_sprite_sheet(moved.m_sprite_sheet)
	, m_current_clip_id(std::move(moved.m_current_clip_id))
	, m_clips(std::move(moved.m_clips))
{
	moved.m_sprite_sheet = nullptr;
}

void sprite_anim::advance_time(float32 delta_sec)
{
	current_clip().advance_time(delta_sec);
}

void sprite_anim::reset()
{
	for(auto& each_clip : m_clips) {
		each_clip.second.reset();
	}
}

void sprite_anim::pause()
{
	current_clip().pause();
}

void sprite_anim::unpause()
{
	current_clip().unpause();
}

const texture2d* sprite_anim::get_texture2d() const
{
	return m_sprite_sheet->m_texture;
}

void sprite_anim::get_current_frame_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const
{
	current_clip().get_current_frame_uvs(top_left, top_right, bottom_left, bottom_right);
}

void sprite_anim::get_current_frame_uvs(vec2 uvs[4]) const
{
	current_clip().get_current_frame_uvs(uvs);
}

bool sprite_anim::set_current_clip(const string& clip_id)
{
	auto found = m_clips.find(clip_id);
	if (found == std::end(m_clips)) {
		return false;
	}
	m_current_clip_id = clip_id;
	found->second.reset();
	return true;
}

void sprite_anim::load_sprite_anim_from_xml(const string& id, const char* path)
{
	xml::document* doc = xml::load_file(path);
	sprite_anim* created = new sprite_anim(doc->child("sprite_anim"));
	xml::unload_file(doc);
	auto found = s_sprite_anim_cache.find(id);
	if (found != std::end(s_sprite_anim_cache)) {
		delete found->second;
	}
	s_sprite_anim_cache[id] = created;
}

sprite_anim* sprite_anim::create_sprite_anim_copy_by_id(const string& id)
{
	const auto found = s_sprite_anim_cache.find(id);
	ASSERT(found != std::end(s_sprite_anim_cache),
		format("No sprite animation loaded with id [%s]", id.c_str()));
	sprite_anim* copied = new sprite_anim(*(found->second));
	return copied;
}

void sprite_anim::clear_cache()
{
	for (auto& each: s_sprite_anim_cache) {
		delete each.second;
	}
	s_sprite_anim_cache.clear();
}
}
