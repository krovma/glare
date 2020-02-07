#include "glare/render/texture.h"
#include "glare/render/renderer.h"
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
