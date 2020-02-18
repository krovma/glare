#include "glare/render/texture.h"
#include "glare/render/renderer.h"
#include "glare/render/surface.h"
#include "core/assert.h"

namespace glare
{
texture2d::texture2d(renderer* r, dx_texture2d* ref_dx_texture)
	: texture(r)
{
	dx_device* device = r->get_dx_device();
	m_texture_usage = TEXTURE_SHADER_RESOURCE | TEXTURE_RENDER_TARGET;
	m_memory_usage = GPU_MEMORY_GPU;

	D3D11_TEXTURE2D_DESC desc;
	ref_dx_texture->GetDesc(&desc);
	if (desc.Format == DXGI_FORMAT_R24G8_TYPELESS) {
		m_texture_usage =  TEXTURE_DEPTH_STENCIL | TEXTURE_SHADER_RESOURCE;
	}
	desc.Usage = static_cast<D3D11_USAGE>(GPU_MEMORY_GPU);
	desc.BindFlags = m_texture_usage;

	dx_texture2d* created;
	HRESULT hr = device->CreateTexture2D(&desc, nullptr, &created);
	if (SUCCEEDED(hr)) {
		m_size = { static_cast<int32>(desc.Width), static_cast<int32>(desc.Height)};
		m_handle = created;
	} else {
		FATAL("Creating texture from reference texture failed");
	}
}

texture2d::texture2d(renderer* r, const surface* from_surface)
	: texture(r)
{
	dx_device* device = r->get_dx_device();
	m_texture_usage = TEXTURE_SHADER_RESOURCE;
	m_memory_usage	= GPU_MEMORY_IMMUTABLE;
	m_size			= from_surface->m_size;
	const bool use_rgba8 = (from_surface->m_raw && from_surface->m_raw_channels == 4);
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width	= m_size.u;
	desc.Height = m_size.v;
	desc.MipLevels	= 1;
	desc.ArraySize	= 1;
	desc.Usage		= static_cast<D3D11_USAGE>(m_memory_usage);
	desc.Format		= 
		use_rgba8
		?	DXGI_FORMAT_R8G8B8A8_UNORM
		:	DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.BindFlags	= static_cast<D3D11_BIND_FLAG>(m_texture_usage);
	desc.CPUAccessFlags	= 0;
	desc.MiscFlags		= 0;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality	= 0;

	const uint32 pitch = 
		use_rgba8
		?	sizeof(rgba8) * m_size.u
		:	sizeof(rgba) * m_size.u;
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem		= 
		use_rgba8
		?	static_cast<void*>(from_surface->m_raw)
		:	from_surface->get_surface_buffer();
	data.SysMemPitch	= pitch;
	HRESULT hr = device->CreateTexture2D(&desc, &data, reinterpret_cast<dx_texture2d**>(&m_handle));
	if (FAILED(hr)) {
		FATAL("Creating texture from surface failed");
	}
}

void texture2d::wrap_dx_texture(dx_texture2d* to_wrap)
{
	D3D11_TEXTURE2D_DESC desc;
	to_wrap->GetDesc(&desc);
	m_memory_usage = static_cast<e_gpu_memory_usage>(desc.Usage);
	m_texture_usage = desc.BindFlags;
	m_size = { static_cast<int32>(desc.Width), static_cast<int32>(desc.Height) };
	m_handle = to_wrap;
	m_handle->AddRef();
}

dx_srv* texture2d::get_view_handle() const
{
	if (!m_handle) {
		// Not raising error here, for async loading
		return nullptr;
	}
	if (!m_srv) {
		create_view();
	}
	return m_srv;
}

void texture2d::create_view() const
{
	dx_device* device = m_renderer->get_dx_device();
	D3D11_TEXTURE2D_DESC desc;
	auto p_texture = static_cast<dx_texture2d*>(m_handle);
	p_texture->GetDesc(&desc);
	HRESULT hr;
	if (desc.Format == DXGI_FORMAT_R24G8_TYPELESS) {
		// depth stencil
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(m_handle, &srv_desc, &m_srv);
	} else {
		hr = device->CreateShaderResourceView(m_handle, nullptr, &m_srv);
	}
	ASSERT(SUCCEEDED(hr), "Failed to create shader resource view from texture2d");
}
}
