#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"

namespace glare
{
class renderer;
class sampler
{
public:
	static void init(renderer* r);
	static dx_sampler* get_sampler(e_texture_filter filter_type);
	static dx_sampler* get_point_sampler();
	static dx_sampler* get_linear_sampler();
public:
	static renderer* s_the_renderer;

private:
	struct point_sampler
	{
		point_sampler(renderer* r);
		~point_sampler();
		dx_sampler* m_handle = nullptr;
	};
	struct linear_sampler
	{
		linear_sampler(renderer* r);
		~linear_sampler();
		dx_sampler* m_handle = nullptr;
	};
};
}
