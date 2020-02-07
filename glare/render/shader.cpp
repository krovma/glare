#include "glare/render/shader.h"
#include "glare/render/renderer.h"
#include "glare/core/assert.h"
#include "glare/core/log.h"
#include "glare/core/string_utilities.h"
#include "glare/data/xml_utils.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

////////////////////////////////////////////////////////
static glare::dx_bytecode* _compile_hlsl_shader(const void* code, size_t code_size, const char* entry_point, const char* shader_model, const char* filename)
{
	DWORD compile_flags = 0U;
#if GLARE_RENDERER_DEBUG_LEVEL >= GLARE_RENDERER_DEBUG_SHADER
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
	glare::dx_bytecode* bytecode = nullptr;
	ID3DBlob* error = nullptr;

	HRESULT hr = ::D3DCompile(code, code_size, filename, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point, shader_model, compile_flags, 0, &bytecode, &error);

	if (FAILED(hr) || error) {
		if (error) {
			char* what = (char*) error->GetBufferPointer();
			glare::debug_log("Compiling %s failed.\n(%s)",filename, what);
			DX_RELEASE(error);
		} else {
			glare::debug_log("Compiling %s failed.\nHRESULT=%u",filename, hr);
		}
	}
	return bytecode;
}

static DXGI_FORMAT _get_dxgi_format(glare::e_hlsl_type type)
{
	switch(type) {
	case glare::HLSL_NULL:
		return DXGI_FORMAT_UNKNOWN;
	case glare::HLSL_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case glare::HLSL_FLOAT2:
		return DXGI_FORMAT_R32G32_FLOAT;
	case glare::HLSL_FLOAT3:
		return DXGI_FORMAT_R32G32B32_FLOAT;
	case glare::HLSL_FLOAT4:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	return DXGI_FORMAT_UNKNOWN;
}

///////////////////////////////////////////////////////

namespace glare
{ 
STATIC shader_stage::~shader_stage()
{
	DX_RELEASE(m_handle);
	DX_RELEASE(m_bytecode);
}

const char* shader_stage::get_shader_model(e_shader_stage stage)
{
	switch(stage) {
	case VERTEX_SHADER:
		return "vs_5_0";
	case PIXEL_SHADER:
		return "ps_5_0";
	default:
		FATAL("Unimplemented shader stage");
	}
	return "LOGIC_IMPOSSIBLE";
}

void shader_stage::compile(
	const renderer* r, const string& src, e_shader_stage stage, const char* filename, const char* entry_point)
{
	dx_bytecode* bytecode = _compile_hlsl_shader(src.c_str(), src.length(), entry_point, get_shader_model(stage), filename);
	if (!bytecode) {
		FATAL(format("Compiling %s failed.", filename));
	}
	m_bytecode = bytecode;
	m_stage = stage;
	HRESULT hr;
	dx_device* device = r->get_dx_device();
	switch(stage) {
	case VERTEX_SHADER: {
		hr = device->CreateVertexShader(m_bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &m_vertex_shader);
		break;
	}
	case PIXEL_SHADER: {
		hr = device->CreatePixelShader(m_bytecode->GetBufferPointer(), bytecode->GetBufferSize(), nullptr, &m_pixel_shader);
		break;
	}
	default:
		FATAL("Shader stage not supported for compiling");
	}
	if (FAILED(hr)) {
		FATAL("Creating shader stage from bytecode failed");
	}
}

//////////////////SHADER//////////////////////////

shader* shader::load_from_xml(const char* path, renderer* r)
{
	shader* created = new shader(r);
	xml::document* doc = xml::load_file(path);

	xml::node pass = doc->child("shader").child("pass");
	xml::node vert = pass.child("vert");
	xml::node pixel = pass.child("pixel");
	string src_file = xml::get_attr(pass, "src", "");
	string vs_entry = xml::get_attr(vert, "vs", "Vert");
	string ps_entry = xml::get_attr(pixel, "ps", "Pixel");
	bool cr = created->load_hlsl(src_file.c_str(), vs_entry.c_str(), ps_entry.c_str());
	ASSERT(cr, "Loading xml shader failed");
	xml::node depth = pass.child("depth");
	created->m_write_depth = xml::get_attr(depth, "write", true);
	string comp_op = xml::get_attr(depth, "comp", "g");
	created->m_depth_comp_op = [&]() {
		if (comp_op == "l") {
			return COMP_LESS;
		} else if( comp_op == "leq") {
			return COMP_LESS_EQ;
		} else if (comp_op == "eq") {
			return COMP_EQ;
		} else if (comp_op == "g") {
			return COMP_GREATER;
		} else if (comp_op == "geq") {
			return COMP_GREATER_EQ;
		}
		return COMP_ALWAYS;
	}();

	xml::node blend = pass.child("blend");
	string blend_mode = xml::get_attr(blend, "mode", "alpha");
	created->m_blend_mode = [&]() {
		if (blend_mode == "alpha") {
			return BLEND_ALPHA;
		} else if (blend_mode == "opaque") {
			return BLEND_OPAQUE;
		} else if (blend_mode == "add") {
			return BLEND_ADDITIVE;
		}
		return BLEND_ALPHA;
	}();

	xml::node raster = pass.child("raster");
	string cull = xml::get_attr(raster, "cull", "back");
	string fill = xml::get_attr(raster, "fill", "solid");
	created->m_front_ccw = xml::get_attr(raster, "front_ccw", true);
	created->m_fill_mode = [&]() {
		if (fill == "wire") {
			return FILL_WIRE;
		}
		return FILL_SOLID;
	}();
	created->m_cull_mode = [&] {
		if (cull == "none") {
			return CULL_NONE;
		} else if (cull == "front") {
			return CULL_FRONT;
		}
		return CULL_BACK;
	}();

	created->m_update_blend_mode = true;
	created->m_update_rasterizer = true;
	created->m_update_depth_stencil = true;
	created->update_all_mode();
	xml::unload_file(doc);
	return created;
}

shader::shader(renderer* r)
	: m_renderer(r)
{
	reset_shader_mode();
}

shader::~shader()
{
	DX_RELEASE(m_vbo_layout);
	DX_RELEASE(m_dx_blend_state);
	DX_RELEASE(m_dx_rasterizer_state);
	DX_RELEASE(m_dx_depth_stencil_state);
}

bool shader::load_hlsl(const char* path, const char* vertex_entry, const char* pixel_entry)
{
	std::string src;
	load_file_to_string(src, path);
	m_vs.compile(m_renderer, src, shader_stage::VERTEX_SHADER, path, vertex_entry);
	m_ps.compile(m_renderer, src, shader_stage::PIXEL_SHADER, path, pixel_entry);
	return is_valid();
}

bool shader::is_valid() const
{
	return m_vs.is_valid() && m_ps.is_valid();
}

void shader::create_dx_vbo_layout(const buffer_layout* layout)
{
	ASSERT(layout, "Invalid buffer layout object");
	DX_RELEASE(m_vbo_layout);
	const size_t attr_count = layout->get_attributes_count();
	D3D11_INPUT_ELEMENT_DESC* desc = new D3D11_INPUT_ELEMENT_DESC[attr_count];
	for (size_t i = 0; i < attr_count; ++i) {
		auto& item = (*layout)[i];
		desc[i].SemanticName = item.name.c_str();
		desc[i].SemanticIndex = 0;
		desc[i].Format = _get_dxgi_format(item.type);
		desc[i].InputSlot = 0;
		desc[i].AlignedByteOffset = item.offset;
		desc[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[i].InstanceDataStepRate = 0;
	}
	dx_bytecode* vs_bytecode = m_vs.get_bytecode();
	HRESULT hr =
		m_renderer->get_dx_device()
			->CreateInputLayout(desc, attr_count, vs_bytecode->GetBufferPointer(), vs_bytecode->GetBufferSize(), &m_vbo_layout);
	delete[] desc;
	if (FAILED(hr)) {
		FATAL("Creating vbo input layout failed");
	}
}

void shader::set_blend_mode(e_blend_mode blend_mode)
{
	m_blend_mode = blend_mode;
}

void shader::update_blend_mode()
{
	if (!m_update_blend_mode) {
		return;
	}
	DX_RELEASE(m_dx_blend_state);
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	desc.RenderTarget[0].BlendEnable = true;
	if (m_blend_mode == BLEND_ALPHA) {
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	} else if (m_blend_mode == BLEND_OPAQUE) {
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	} else if (m_blend_mode == BLEND_ADDITIVE) {
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	}
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr=
		m_renderer->get_dx_device()
			->CreateBlendState(&desc, &m_dx_blend_state);
	if (SUCCEEDED(hr)) {
		m_update_blend_mode = false;
	} else {
		FATAL("Creating blend state failed");
	}
}

void shader::set_depth_stencil_mode(e_compare_operator pass_op, bool write)
{
	m_depth_comp_op = pass_op;
	m_write_depth	= write;
	m_update_depth_stencil = true;
}

void shader::update_depth_stencil_mode()
{
	if (!m_update_depth_stencil) {
		return;
	}
	DX_RELEASE(m_dx_depth_stencil_state);
	D3D11_DEPTH_STENCIL_DESC ds_desc;
	memset(&ds_desc, 0, sizeof(ds_desc));
	ds_desc.DepthEnable = TRUE;
	ds_desc.DepthWriteMask	=
		m_write_depth ?
		D3D11_DEPTH_WRITE_MASK_ALL :
		D3D11_DEPTH_WRITE_MASK_ZERO;
	ds_desc.DepthFunc		= static_cast<D3D11_COMPARISON_FUNC>(m_depth_comp_op);
	ds_desc.StencilEnable	= FALSE;
	ds_desc.StencilReadMask	= 0xff;
	ds_desc.StencilWriteMask	= 0xff;
	D3D11_DEPTH_STENCILOP_DESC ds_op; {
		memset(&ds_op, 0, sizeof(ds_op));
		ds_op.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
		ds_op.StencilDepthFailOp	= D3D11_STENCIL_OP_KEEP;
		ds_op.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
		ds_op.StencilFunc			= D3D11_COMPARISON_ALWAYS;
	}
	ds_desc.FrontFace	= ds_op;
	ds_desc.BackFace	= ds_op;
	HRESULT hr =
		m_renderer->get_dx_device()
			->CreateDepthStencilState(&ds_desc, &m_dx_depth_stencil_state);
	if (SUCCEEDED(hr)) {
		m_update_depth_stencil = false;
	} else {
		FATAL("Creating depth stencil state failed");
	}
}

void shader::set_rasterizer_mode(e_cull_mode cull_mode, e_fill_mode fill_mode, bool front_face_ccw, bool depth_clip)
{
	m_cull_mode		= cull_mode;
	m_fill_mode		= fill_mode;
	m_front_ccw		= front_face_ccw;
	m_depth_clip	= depth_clip;
	m_update_rasterizer = true;
}

void shader::update_rasterizer_mode()
{
	if (!m_update_rasterizer) {
		return;
	}
	DX_RELEASE(m_dx_rasterizer_state);
	D3D11_RASTERIZER_DESC rs_desc;
	memset(&rs_desc, 0, sizeof(rs_desc));
	rs_desc.CullMode		= static_cast<D3D11_CULL_MODE>(m_cull_mode);
	rs_desc.FillMode		= static_cast<D3D11_FILL_MODE>(m_fill_mode);
	rs_desc.DepthBias		= 0;
	rs_desc.AntialiasedLineEnable = FALSE;
	rs_desc.FrontCounterClockwise = static_cast<BOOL>(m_front_ccw);
	rs_desc.DepthClipEnable	= static_cast<BOOL>(m_depth_clip);
	HRESULT hr =
		m_renderer->get_dx_device()
			->CreateRasterizerState(&rs_desc, &m_dx_rasterizer_state);
	if (SUCCEEDED(hr)) {
		m_update_rasterizer = false;
	} else {
		FATAL("Creating rastrerizer state failed");
	}
}

void shader::update_all_mode()
{
	update_blend_mode();
	update_depth_stencil_mode();
	update_rasterizer_mode();
}

void shader::reset_shader_mode()
{
	// reset all dx state to default
	
	{// reset accessible state value
		m_blend_mode	= BLEND_ALPHA;
		m_depth_comp_op	= COMP_GREATER;
		m_write_depth	= true;
		m_cull_mode		= CULL_BACK;
		m_fill_mode		= FILL_SOLID;
		m_front_ccw		= true;
		m_depth_clip	= true;

		m_update_blend_mode = true;
		m_update_rasterizer = true;
		m_update_depth_stencil = true;
	}
	update_all_mode();
}

void shader::use_state(dx_state_blend* blend, dx_state_depth_stencil* depth_stencil, dx_state_rasterizer* rasterizer)
{
	m_dx_blend_state = blend;
	m_dx_depth_stencil_state = depth_stencil;
	m_dx_rasterizer_state = rasterizer;
}
};