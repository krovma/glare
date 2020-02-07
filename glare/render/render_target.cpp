#include "glare/render/render_target.h"
#include "core/assert.h"

namespace glare
{
void render_target::make_from_dx_texture(dx_device* device, dx_texture2d* texture)
{
	ASSERT((device && texture), "Not a valid d3d11_device or texture");
	DX_RELEASE(m_rtv);
	HRESULT hr = device->CreateRenderTargetView(texture, nullptr, &m_rtv);
	if (SUCCEEDED(hr)) {
		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);
		m_size = {static_cast<int32>(desc.Width), static_cast<int32>(desc.Height)};
	}
}
};