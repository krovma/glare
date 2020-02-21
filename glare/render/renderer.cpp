#include "glare/render/renderer.h"
#include "glare/core/window.h"
#include "glare/render/shader.h"
#include "glare/render/surface.h"
#include "glare/render/mesh.h"
#include "glare/core/assert.h"
#include "core/string_utils.h"
#include "glare/render/sampler.h"

namespace glare
{
renderer::renderer(const window* client)
{
	m_resolution = client->get_client_resolution();
	uint32 device_creation_flags = 0;
	
#if GLARE_RENDERER_DEBUG_LEVEL >= GLARE_RENDERER_DEBUG_LEAK
	device_creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC swapchain_desc;
	memset(&swapchain_desc, 0, sizeof(swapchain_desc));
	swapchain_desc.BufferCount = 2;
	swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
	swapchain_desc.OutputWindow = static_cast<HWND>(client->m_hwnd);
	swapchain_desc.SampleDesc.Count = 1;
	swapchain_desc.Windowed = TRUE;
	swapchain_desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapchain_desc.BufferDesc.Width = m_resolution.x;
	swapchain_desc.BufferDesc.Height = m_resolution.y;

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		device_creation_flags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapchain_desc,
		&m_swapchain,
		&m_device,
		nullptr,
		&m_context
	);

	ASSERT(SUCCEEDED(hr), "Creating D3D Context failed.");
	m_buffer_vbo = new vertex_buffer(this);
	m_buffer_model = new constant_buffer(this);
	m_buffer_post = new constant_buffer(this);
	m_buffer_project = new constant_buffer(this);
	
#if GLARE_RENDERER_DEBUG_LEVEL >= GLARE_RENDERER_DEBUG_LEAK
	hr = m_device->QueryInterface(IID_PPV_ARGS(&m_debug));
	CHECK(SUCCEEDED(hr), "Can\'t create D3D debug context");
#endif
}

void renderer::start()
{
	sampler::init(this);
	sampler::get_point_sampler();
	sampler::get_linear_sampler();
	// #TODO: default texture
}

void renderer::begin_frame()
{
	dx_texture2d* p_back_buffer;
	m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&p_back_buffer));
	// add back buffer here
	m_frame_back_buffer_texture = new texture2d(this);
	m_frame_back_buffer_texture->wrap_dx_texture(p_back_buffer); // the original texture on swap chain
	m_frame_texture = new texture2d(this, p_back_buffer);		 // a new blank texture for rendering, blit to back buffer at end of frame
	m_frame_render_target = new render_target();
	m_frame_render_target->make_from_dx_texture(m_device, m_frame_texture->get_texture_handle());
	DX_RELEASE(p_back_buffer);

	m_context->OMSetRenderTargets(1, &(m_frame_render_target->m_rtv), nullptr);

	reset_viewport();

	m_buffer_project->buffer(&m_projection, sizeof(m_projection));
	
}

void renderer::end_frame()
{
	copy_texture(m_frame_back_buffer_texture, m_frame_texture);
	// Check Present flags at https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
	m_swapchain->Present(0, 0);
	delete m_frame_render_target;
	m_frame_render_target = nullptr;
	delete m_frame_texture;
	m_frame_texture = nullptr;
	delete m_frame_back_buffer_texture;
	m_frame_back_buffer_texture = nullptr;
}

void renderer::stop()
{
	delete m_buffer_vbo;
	delete m_buffer_model;
	delete m_buffer_post;
	DX_RELEASE(m_swapchain);
	DX_RELEASE(m_context);
	DX_RELEASE(m_device);
#if GLARE_RENDERER_DEBUG_LEVEL >= GLARE_RENDERER_DEBUG_LEAK
	m_debug->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL);
#endif
	DX_RELEASE(m_debug);
	
}

texture2d* renderer::load_texture2d_from_file(const string& id, const char* path, bool flip_v)
{
	const auto found = s_cached_texture.find(id);
	if (found == std::end(s_cached_texture)) {
		surface loaded_surface(path, flip_v);
		texture2d* created = new texture2d(this, &loaded_surface);
		s_cached_texture[id] = created;
		return created;
	}
	return found->second;
}

texture2d* renderer::get_texture2d_by_id(const string& id) const
{
	const auto found = s_cached_texture.find(id);
	if (found == std::end(s_cached_texture)) {
		ALERT(format("texture(id=%s) has not loaded yet.", id.c_str()));
		return nullptr;
	}
	return found->second;
}

void renderer::set_ortho(const vec2& ortho_min, const vec2& ortho_max, float32 near_z, float32 far_z)
{
	const float x_ratio = 1.f / (ortho_max.x - ortho_min.x);
	const float y_ratio = 1.f / (ortho_max.y - ortho_min.y);
	const float z_ratio = 1.f / (far_z - near_z);
	const vec4 i(2.f * x_ratio, 0.f, 0.f, -(ortho_min.x + ortho_max.x) * x_ratio);
	const vec4 j(0.f, 2.f * y_ratio, 0.f, -(ortho_min.y + ortho_max.y) * y_ratio);
	const vec4 k(0.f, 0.f, -z_ratio, far_z * z_ratio);
	const vec4 t(0.f, 0.f, 0.f, 1.f);
	m_projection.i = i;
	m_projection.j = j;
	m_projection.k = k;
	m_projection.t = t;
	m_projection.transpose();
}

void renderer::set_viewport(const ivec2& size, const ivec2& topleft) const
{
	D3D11_VIEWPORT vp;
	vp.Width = size.x;
	vp.Height = size.y;
	vp.TopLeftX = topleft.x;
	vp.TopLeftY = topleft.y;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	m_context->RSSetViewports(1, &vp);
}

void renderer::reset_viewport() const
{
	static D3D11_VIEWPORT vp;
	vp.Width = m_frame_render_target->m_size.x;
	vp.Height = m_frame_render_target->m_size.y;
	vp.TopLeftX = 0u;
	vp.TopLeftY = 0u;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	m_context->RSSetViewports(1, &vp);
}

void renderer::copy_texture(texture* dst, texture* src) const
{
	ASSERT((!dst->is_immutable()), "Trying to write a immutable texture");
	m_context->CopyResource(dst->get_texture_handle(), src->get_texture_handle());
}

void renderer::bind_shader(shader* shader)
{
	m_current_shader = shader;
	m_context->VSSetShader(m_current_shader->get_dx_vs(), nullptr, 0);
	m_context->PSSetShader(m_current_shader->get_dx_ps(), nullptr, 0);
}

void renderer::bind_vbo(vertex_buffer* vbo) const
{
	dx_buffer* buf = vbo?vbo->m_handle:nullptr;
	unsigned stride = vbo?vbo->get_buffer_layout()->get_stride():0;
	unsigned offset = 0;
	m_context->IASetVertexBuffers(0, 1, &buf, &stride, &offset);
}

void renderer::bind_ibo(index_buffer* ibo) const
{
	dx_buffer* buf = ibo ? ibo->m_handle : nullptr;
	m_context->IASetIndexBuffer(buf, index_buffer::format, 0);
}

void renderer::bind_constant_buffer(e_constant_buffer_id buffer_id, constant_buffer* buffer) const
{
	dx_buffer* buf = buffer ? buffer->get_buffer_handle() : nullptr;
	m_context->VSSetConstantBuffers(buffer_id, 1, &buf);
	m_context->PSSetConstantBuffers(buffer_id, 1, &buf);
}

void renderer::bind_texture(const texture* tex, e_texture_slot slot, e_texture_filter filter) const
{
	dx_srv* srv = tex->get_view_handle();
	dx_sampler* sampler = sampler::get_sampler(filter);
	m_context->PSSetShaderResources(slot, 1, &srv);
	m_context->PSSetSamplers(slot, 1, &sampler);
}

void renderer::clear_render_target(const rgba& clear_color) const
{
	m_context->ClearRenderTargetView(m_frame_render_target->get_dx_handle(), reinterpret_cast<const FLOAT*>(&clear_color));
}

void renderer::draw(size_t vertex_count, size_t offset) const
{
	m_current_shader->update_all_mode();
	static float black[] = { 0.f,0.f,0.f,1.f };
	m_context->OMSetBlendState(m_current_shader->m_dx_blend_state, black, 0xffffffff);
	m_context->OMSetDepthStencilState(m_current_shader->m_dx_depth_stencil_state, 0);
	m_context->RSSetState(m_current_shader->m_dx_rasterizer_state);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_current_shader->m_vbo_layout);
	m_context->Draw(vertex_count, offset);
}

void renderer::draw_indexed(size_t indice_count, size_t offset) const
{
	m_current_shader->update_all_mode();
	static float black[] = { 0.f,0.f,0.f,1.f };
	m_context->OMSetBlendState(m_current_shader->m_dx_blend_state, black, 0xffffffff);
	m_context->OMSetDepthStencilState(m_current_shader->m_dx_depth_stencil_state, 0);
	m_context->RSSetState(m_current_shader->m_dx_rasterizer_state);
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_context->IASetInputLayout(m_current_shader->m_vbo_layout);
	m_context->DrawIndexed(indice_count, offset, 0);
}

void renderer::draw_mesh(const mesh* mesh) const
{
	bind_vbo(mesh->m_gpu_vbo);
	m_current_shader->create_dx_vbo_layout(mesh->m_layout);
	if (mesh->is_indexed()) {
		bind_ibo(mesh->m_gpu_ibo);
		draw_indexed(mesh->get_element_count());
	} else {
		draw(mesh->get_element_count());
	}
}

STATIC std::unordered_map<string, texture2d*> renderer::s_cached_texture;
};