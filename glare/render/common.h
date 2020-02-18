#pragma once
#include "glare/core/common.h"
#include <d3d11.h>
#include <dxgi.h>
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace glare {

//#define GLARE_INDEX_T_UINT16
#define GLARE_INDEX_T_UINT32
#define GLARE_RENDERER_DEBUG_LEAK	1
#define GLARE_RENDERER_DEBUG_SHADER 10

#if defined(_DEBUG)
#define GLARE_RENDERER_DEBUG_LEVEL 999
#else
#define GLARE_RENDERER_DEBUG_LEVEL 0
#endif

#define DX_RELEASE(resource)\
{\
	if (resource) {\
		(resource)->Release();\
		(resource) = nullptr;\
	}\
}

using dx_device		= ID3D11Device;
using dx_context	= ID3D11DeviceContext;
using dx_resource	= ID3D11Resource;
using dx_texture2d	= ID3D11Texture2D;
using dx_rtv		= ID3D11RenderTargetView;
using dx_srv		= ID3D11ShaderResourceView;
using dx_swapchain	= IDXGISwapChain;
using dx_debug		= ID3D11Debug;
using dx_buffer		= ID3D11Buffer;
using dx_bytecode	= ID3D10Blob;
using dx_vs			= ID3D11VertexShader;
using dx_ps			= ID3D11PixelShader;
using dx_layout		= ID3D11InputLayout;
using dx_state_depth_stencil	= ID3D11DepthStencilState;
using dx_state_rasterizer		= ID3D11RasterizerState;
using dx_state_blend			= ID3D11BlendState;
using dx_sampler	= ID3D11SamplerState;
using dx_format		= DXGI_FORMAT;

enum e_render_buffer_usage
{
	RENDER_BUFFER_VERTEX	= D3D11_BIND_VERTEX_BUFFER,
	RENDER_BUFFER_INDEX		= D3D11_BIND_INDEX_BUFFER,
	RENDER_BUFFER_CONSTANT	= D3D11_BIND_CONSTANT_BUFFER,
};

enum e_gpu_memory_usage
{
	GPU_MEMORY_GPU			= D3D11_USAGE_DEFAULT,
	GPU_MEMORY_IMMUTABLE	= D3D11_USAGE_IMMUTABLE,
	GPU_MEMORY_DYNAMIC		= D3D11_USAGE_DYNAMIC,
	GPU_MEMORY_STAGING		= D3D11_USAGE_STAGING
};

enum e_texture_usage
{
	TEXTURE_SHADER_RESOURCE = D3D11_BIND_SHADER_RESOURCE,
	TEXTURE_RENDER_TARGET	= D3D11_BIND_RENDER_TARGET,
	TEXTURE_DEPTH_STENCIL	= D3D11_BIND_DEPTH_STENCIL
};

enum e_texture_slot : uint32
{
	TEXTURE_SLOT_DIFFUSE = 0,
	TEXTURE_SLOT_NORMAL	 = 1,
	TEXTURE_SLOT_EMMISIVE = 2,
	TEXTURE_SLOT_SPECULAR = 3,
	TEXTURE_SLOT_ROUGHNESS = 4,
	TEXTURE_SLOT_METALLIC = 5
};

enum e_constant_buffer_id : uint32
{
	CONSTANT_FRAME_BUFFER	= 1,
	CONSTANT_CAMERA_BUFFER	= 2,
	CONSTANT_MODEL_BUFFER	= 3,
	CONSTANT_LIGHT_BUFFER	= 4,
	CONSTANT_EFFECT_BUFFER	= 8,
};

enum e_blend_mode
{
	BLEND_OPAQUE,
	BLEND_ALPHA,
	BLEND_ADDITIVE
};

enum e_texture_filter
{
	MIN_POINT_MAG_POINT = D3D11_FILTER_MIN_MAG_MIP_POINT,
	MIN_POINT_MAG_LINEAR = D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
	MIN_LINEAR_MAG_POINT = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT,
	MIN_LINEAR_MAG_LINEAR = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT
};

enum e_compare_operator
{
	COMP_NEVER		= D3D11_COMPARISON_NEVER,
	COMP_ALWAYS		= D3D11_COMPARISON_ALWAYS,
	COMP_EQ			= D3D11_COMPARISON_EQUAL,
	COMP_NEQ		= D3D11_COMPARISON_NOT_EQUAL,
	COMP_LESS		= D3D11_COMPARISON_LESS,
	COMP_LESS_EQ	= D3D11_COMPARISON_LESS_EQUAL,
	COMP_GREATER	= D3D11_COMPARISON_GREATER,
	COMP_GREATER_EQ	= D3D11_COMPARISON_GREATER_EQUAL
};

enum e_cull_mode
{
	CULL_NONE	= D3D11_CULL_NONE,
	CULL_FRONT	= D3D11_CULL_FRONT,
	CULL_BACK	= D3D11_CULL_BACK
};

enum e_fill_mode
{
	FILL_WIRE		= D3D11_FILL_WIREFRAME,
	FILL_SOLID		= D3D11_FILL_SOLID
};
};