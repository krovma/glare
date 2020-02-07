#include "glare/render/vertex.h"

namespace glare
{
STATIC buffer_layout::attribute vertex_pcu::_s_attr[] = {
	{"POSITION",	HLSL_FLOAT3, offsetof(vertex_pcu, position)},
	{"COLOR",		HLSL_FLOAT4, offsetof(vertex_pcu, color)},
	{"TEXCOORD",	HLSL_FLOAT2, offsetof(vertex_pcu, uv)},
	buffer_layout::attribute::END()
};
};