#pragma once
#include "glare/core/common.h"
#include "glare/render/texture.h"
#include "glare/data/xml_utils.h"
#include "glare/math/aabb2.h"
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
	NODISCARD virtual const texture2d* get_texture2d() const = 0;
	virtual void get_current_frame_uvs(
		vec2& top_left, vec2& top_right,
		vec2& bottom_left, vec2& bottom_right) const = 0;
};

// A static sprite can also be used as an animation
class sprite : public i_sprite_anim_base
{
public:
	sprite() = default;
	sprite(const sprite& copy)
		: m_sprite_sheet(copy.m_sprite_sheet)
		, m_bottom_left(copy.m_bottom_left)
		, m_top_right(copy.m_top_right)
	{}
	~sprite() override
	{
		/* no data allocation*/
	}
	sprite(sprite_sheet* sheet, const vec2& bottom_left, const vec2& top_right)
		: m_sprite_sheet(sheet), m_bottom_left(bottom_left), m_top_right(top_right)
	{}

	void get_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const;

	void advance_time(float32 delta_sec) override
	{
		UNUSED(delta_sec);
	}
	
	void reset() override
	{
		/* Do nothing */
	}
	
	NODISCARD const texture2d* get_texture2d() const override;

	void get_current_frame_uvs(vec2& top_left, vec2& top_right, vec2& bottom_left, vec2& bottom_right) const override
	{
		get_uvs(top_left, top_right, bottom_left, bottom_right);
	}
	
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
	NODISCARD sprite* get_sprite_copy(const ivec2& coord)
	{
		return get_sprite_copy(coord.v * m_layout.u + coord.u);
	}
	//irregular layout constructor
	sprite_sheet(renderer* r, xml::document* xml_doc);
	//irregular layout getter
	NODISCARD const sprite& get_sprite(size_t index) const
	{
		return m_sprites[index];
	}
	NODISCARD sprite* get_sprite_copy(size_t index)
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
};

class sprite_anim : public i_sprite_anim_base
{
	
};

}
