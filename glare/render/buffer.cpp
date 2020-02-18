#include "glare/render/buffer.h"
#include "glare/render/renderer.h"
#include "core/assert.h"

namespace glare
{

const buffer_layout* buffer_layout::acquire_layout(
	const attribute attr[], size_t stride, super_vertex_copier copy_func)
{
	buffer_layout* created = new buffer_layout(attr);
	created->m_stride = stride;
	created->m_copy_func = copy_func;
	return created;
}

buffer_layout::buffer_layout(const buffer_layout::attribute attr[])
{
	m_stride = 0;
	for (auto p = attr; !(p->is_null());++p) {
		m_attributes.push_back(*p);
		m_stride += get_size_of_hlsl_type(p->type);
	}
}

render_buffer::render_buffer(renderer* r)
	: m_renderer(r)
{
}

render_buffer::~render_buffer()
{
	DX_RELEASE(m_handle);
}

bool render_buffer::create(
	const void* data, size_t buffer_size, size_t element_size, e_render_buffer_usage buffer_usage
	, e_gpu_memory_usage memory_usage)
{
	if (buffer_size == 0 || !data) {
		return false;
	}
	DX_RELEASE(m_handle);
	ASSERT(!is_immutable(), "immutable buffer must be created from *::create_immutable_buffer");

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = static_cast<UINT>(buffer_size);
	desc.StructureByteStride = static_cast<UINT>(element_size);
	desc.Usage = static_cast<D3D11_USAGE>(memory_usage);
	desc.BindFlags = static_cast<D3D11_BIND_FLAG>(buffer_usage);
	if (memory_usage == GPU_MEMORY_DYNAMIC) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	} else if (memory_usage == GPU_MEMORY_STAGING) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	} else if (memory_usage == GPU_MEMORY_IMMUTABLE) {
		desc.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA data_desc;
	memset(&data_desc, 0, sizeof(data_desc));
	data_desc.pSysMem = data;
	dx_device* device = m_renderer->get_dx_device();
	HRESULT hr = device->CreateBuffer(&desc, &data_desc, &m_handle);
	if (SUCCEEDED(hr)) {
		m_buffer_usage = buffer_usage;
		m_memory_usage = memory_usage;
		m_buffer_size = buffer_size;
		m_element_size = element_size;
	} else {
		FATAL("Creating D3D11Buffer Failed");
	}
	return true;
}

bool render_buffer::buffer(const void* data, size_t size)
{
	if (size == 0) {
		return false;
	}
	ASSERT(!is_immutable(), "Buffering an immutable buffer is prohibited.");
	ASSERT((m_buffer_size >= size), "Not enough gpu buffer.");
	dx_context* ctx = m_renderer->get_dx_context();
	D3D11_MAPPED_SUBRESOURCE map;
	HRESULT hr = ctx->Map(m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
	if (SUCCEEDED(hr)) {
		memcpy(map.pData, data, size);
		ctx->Unmap(m_handle, 0);
		return true;
	}
	ALERT("Mapping gpu resource failed. Abort buffering.");
	return false;
}

constant_buffer::constant_buffer(renderer* r)
	:render_buffer(r)
{
	m_memory_usage = GPU_MEMORY_DYNAMIC;
	m_buffer_usage = RENDER_BUFFER_CONSTANT;
}

bool constant_buffer::buffer(const void* data, size_t size)
{
	if (size > m_buffer_size || is_immutable()) {
		return create( data, size, size, RENDER_BUFFER_CONSTANT, GPU_MEMORY_DYNAMIC);
	}
	return render_buffer::buffer(data, size);
}

vertex_buffer::vertex_buffer(renderer* r)
	:render_buffer(r)
{
	m_memory_usage = GPU_MEMORY_DYNAMIC;
	m_buffer_usage = RENDER_BUFFER_VERTEX;
}

bool vertex_buffer::buffer(const void* data, size_t count)
{
	const size_t size = count * m_buffer_layout->get_stride();
	bool result;
	if (size > m_buffer_size || is_immutable()) {
		result = create(data, size, m_buffer_layout->get_stride(), RENDER_BUFFER_VERTEX, GPU_MEMORY_DYNAMIC);
	} else {
		result = render_buffer::buffer(data, size);
	}
	if (result) {
		m_count = count;
	}
	return result;
}

bool vertex_buffer::create_immutable_buffer(const void* data, size_t vertex_count, const buffer_layout* layout)
{
	const bool result = create(data, layout->get_stride() * vertex_count, layout->get_stride(), RENDER_BUFFER_VERTEX, GPU_MEMORY_IMMUTABLE);
	if (result) {
		m_count = vertex_count;
		m_buffer_layout = layout;
	}
	return result;
}

index_buffer::index_buffer(renderer* r)
	:render_buffer(r)
{
	m_memory_usage = GPU_MEMORY_DYNAMIC;
	m_buffer_usage = RENDER_BUFFER_INDEX;
}


bool index_buffer::buffer(const void* data, size_t count)
{
	const size_t size = count * sizeof(index_t);
	bool result;
	if (size > m_buffer_size || is_immutable()) {
		result = create(data, size, sizeof(index_t), RENDER_BUFFER_INDEX, GPU_MEMORY_DYNAMIC);
	} else {
		result = render_buffer::buffer(data, size);
	}
	if (result) {
		m_count = count;
	}
	return result;
}

bool index_buffer::create_immutable_buffer(const index_t* data, size_t count)
{
	const bool result = create(data, count * sizeof(index_t), sizeof(index_t), RENDER_BUFFER_INDEX, GPU_MEMORY_IMMUTABLE);
	if (result) {
		m_count = count;
	}
	return result;
}

////// STATICS //////

STATIC buffer_layout::attribute buffer_layout::attribute::END()
{
	static attribute _END;
	return _END;
}
STATIC std::unordered_map<std::type_index, const buffer_layout*> buffer_layout::s_cached_layout;
};
