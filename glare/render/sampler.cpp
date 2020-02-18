#include "glare/render/sampler.h"
#include "glare/render/renderer.h"
#include "core/assert.h"

namespace glare
{

STATIC renderer* sampler::s_the_renderer = nullptr;

void sampler::init(renderer* r)
{
	s_the_renderer = r;
}

dx_sampler* sampler::get_sampler(e_texture_filter filter_type)
{
	switch (filter_type) {
	case MIN_POINT_MAG_POINT:
		return get_point_sampler();
	case MIN_LINEAR_MAG_LINEAR:
		return get_linear_sampler();
	case MIN_POINT_MAG_LINEAR:
	case MIN_LINEAR_MAG_POINT:
	default:
		return get_point_sampler();
	}
}

dx_sampler* sampler::get_point_sampler()
{
	static point_sampler the_point_sampler(s_the_renderer);
	return the_point_sampler.m_handle;
}

dx_sampler* sampler::get_linear_sampler()
{
	static linear_sampler the_linear_sampler(s_the_renderer);
	return the_linear_sampler.m_handle;
}

sampler::point_sampler::point_sampler(renderer* r)
{
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Filter = static_cast<D3D11_FILTER>(MIN_POINT_MAG_POINT);
	desc.MaxAnisotropy = 1u;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	desc.MipLODBias = 0.f;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	const HRESULT hr = r->get_dx_device()->CreateSamplerState(&desc, &m_handle);
	ASSERT(SUCCEEDED(hr), "Creating sampler state failed");
}

sampler::point_sampler::~point_sampler()
{
	DX_RELEASE(m_handle);
}

sampler::linear_sampler::linear_sampler(renderer* r)
{
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Filter = static_cast<D3D11_FILTER>(MIN_LINEAR_MAG_LINEAR);
	desc.MaxAnisotropy = 1u;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	desc.MipLODBias = 0.f;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	const HRESULT hr = r->get_dx_device()->CreateSamplerState(&desc, &m_handle);
	ASSERT(SUCCEEDED(hr), "Creating sampler state failed");
}

sampler::linear_sampler::~linear_sampler()
{
	DX_RELEASE(m_handle);
}
}
