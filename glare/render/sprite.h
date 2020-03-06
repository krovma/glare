#pragma once
#include "glare/core/common.h"
#include "glare/render/texture.h"
#include "glare/data/xml_utils.h"
#include "glare/math/aabb2.h"
#include <unordered_map>
#include <vector>

namespace glare
{
class i_sprite_anim_base;
class sprite_sheet;
class sprite_anim;
class sprite;
class renderer;

class i_sprite_anim_base
{
public:
	virtual			~i_sprite_anim_base() = 0;
	virtual void	advance_time(float32 delta_sec) = 0;
	virtual void	reset() = 0;
	virtual void	pause() = 0;
	virtual void	unpause() = 0;
	virtual bool	set_current_clip(const string& clip_id) = 0;
	NODISCARD virtual const texture2d* get_texture2d() const = 0;
	virtual void get_current_frame_uvs(
		vec2& top_left, vec2& top_right,
		vec2& bottom_left, vec2& bottom_right) const = 0;
	virtual void get_current_frame_uvs(vec2 uvs[4]) const = 0;
};

// A static sprite can also be used as an animation
class sprite final : public i_sprite_anim_base
{
public:
	sprite() = default;
	sprite(const sprite& copy)
		: m_sprite_sheet(copy.m_sprite_sheet)
		, m_bottom_left(copy.m_bottom_left)
		, m_top_right(copy.m_top_right)
	{}
	sprite(sprite&& moved) noexcept
		: m_sprite_sheet(moved.m_sprite_sheet)
		, m_bottom_left(moved.m_bottom_left)
		, m_top_right(moved.m_top_right)
	{
		moved.m_sprite_sheet = nullptr;
	}
	sprite& operator=(const sprite& copy)
	{
		m_sprite_sheet = copy.m_sprite_sheet;
		m_bottom_left = copy.m_bottom_left;
		m_top_right = copy.m_top_right;
		return *this;
	}
	~sprite() override
	{
		/* no data allocation*/
	}
	sprite(sprite_sheet* sheet, const vec2& bottom_left, const vec2& top_right)
		: m_sprite_sheet(sheet), m_bottom_left(bottom_left), m_top_right(top_right)
	{}

	void get_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const;

	void get_current_frame_uvs(vec2 uvs[4]) const override final;
	void get_current_frame_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const override final
	{
		get_uvs(top_left, top_right, bottom_left, bottom_right);
	}

	void advance_time(float32 delta_sec) override
	{
		UNUSED(delta_sec);
	}
	
	void reset() override
	{
		/* Do nothing */
	}

	void pause() override
	{
		/* Do nothing */
	}
	void unpause() override
	{
		/* Do nothing */
	}

	bool set_current_clip(const string& clip_id) override
	{
		UNUSED(clip_id);
		return true;
	}
	NODISCARD const texture2d* get_texture2d() const override;
	
public:
	sprite_sheet* m_sprite_sheet = nullptr;
	vec2	m_bottom_left;
	vec2	m_top_right;
};

class sprite_sheet
{
public:
	sprite_sheet() = default;
	//regular layout constructor
	sprite_sheet(texture2d* texture, const ivec2& layout);
	//regular layout getter
	NODISCARD const sprite& get_sprite(const ivec2& coord) const
	{
		return get_sprite(coord.v * m_layout.u + coord.u);
	}
	NODISCARD sprite* create_sprite_copy(const ivec2& coord)
	{
		return create_sprite_copy(coord.v * m_layout.u + coord.u);
	}
	//irregular layout constructor
	sprite_sheet(renderer* r, xml::document* xml_doc);
	//irregular layout getter
	NODISCARD const sprite& get_sprite(size_t index) const
	{
		return m_sprites[index];
	}
	NODISCARD sprite* create_sprite_copy(size_t index)
	{
		sprite* copied = new sprite(m_sprites[index]);
		return copied;
	}

	NODISCARD size_t get_num_sprites() const
	{
		return m_sprites.size();
	}
public:
	const texture2d* m_texture = nullptr;
	ivec2 m_layout;
	std::vector<sprite>	m_sprites;
public:
	static std::unordered_map<string, sprite_sheet*> s_sprite_sheet_cache;
	static void load_sprite_sheet_from_texture(const string& id, const ivec2& layout, texture2d* texture);
	static void load_sprite_sheet_from_xml(const string& id, renderer* r, const char* path);
	static sprite_sheet* get_sprite_sheet(const string& id);
	static void clear_cache();
};

class sprite_anim_clip
{
public:
	enum class e_playback_mode
	{
		PLAYBACK_ONCE,
		PLAYBACK_LOOP,
		PLAYBACK_PINGPONG
	};
	struct frame
	{
		sprite m_sprite;
		float32 m_frame_time = 0.f;
		frame() = default;
		frame(sprite sprite_copy, float32 frame_time)
			: m_sprite(std::move(sprite_copy)), m_frame_time(frame_time)
		{}
	};
public:
	sprite_anim_clip() = default;
	sprite_anim_clip(std::vector<frame> frames, bool start_paused=false);
	void get_current_frame_uvs(vec2 uvs[4]) const;
	void get_current_frame_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const;
	void advance_time(float32 delta_sec);
	void pause();
	void unpause();
	void reset();
public:
	int32		m_total_frame = 0;
	int32		m_current_frame = 0;
	float		m_current_frame_rest_time = 0;
	bool		m_paused = false;
	bool		m_forward = true;
	e_playback_mode m_playback_mode = e_playback_mode::PLAYBACK_LOOP;
	std::vector<frame> m_frames;
};

class sprite_anim final: public i_sprite_anim_base
{
public:
	sprite_anim() = default;
	sprite_anim(const xml::node& xml);
	sprite_anim(const sprite_anim& copy_from);
	sprite_anim(sprite_anim&& moved) noexcept;
	~sprite_anim() = default; // no ownership of resources
	
	void advance_time(float32 delta_sec) override;
	void reset() override;
	void pause() override;
	void unpause() override;
	NODISCARD const texture2d* get_texture2d() const override;
	void get_current_frame_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const override;
	void get_current_frame_uvs(vec2 uvs[4]) const override;

	bool set_current_clip(const string& clip_id) override;

	NODISCARD sprite_anim_clip& current_clip() const
	{
		return const_cast<sprite_anim_clip&>(m_clips.find(m_current_clip_id)->second);
	}
public:
	sprite_sheet*	m_sprite_sheet = nullptr;
	string	m_current_clip_id = "UNSET";
	std::unordered_map<string, sprite_anim_clip> m_clips;
public:
	static std::unordered_map<string, sprite_anim*> s_sprite_anim_cache;
	static void load_sprite_anim_from_xml(const string& id, const char* path);
	static sprite_anim* create_sprite_anim_copy_by_id(const string& id);
	static void clear_cache();
};

}
