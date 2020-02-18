
#pragma once
#include "glare/core/common.h"
#include "glare/math/vector.h"

namespace glare
{
// ReSharper disable CppInconsistentNaming
enum e_mat4_index
{
	ix=0, iy, iz, iw,
	
	jx, jy, jz, jw,
	kx, ky ,kz ,kw,
	tx, ty, tz, tw
};
// ReSharper restore CppInconsistentNaming
struct mat4
{
	union
	{
		float32 m_value[16];
		struct
		{
			float32 _ix; float32 _iy; float32 _iz; float32 _iw;
			float32 _jx; float32 _jy; float32 _jz; float32 _jw;
			float32 _kx; float32 _ky; float32 _kz; float32 _kw;
			float32 _tx; float32 _ty; float32 _tz; float32 _tw;
		};
		struct
		{
			vec4 i;
			vec4 j;
			vec4 k;
			vec4 t;
		};
	};

	mat4()
	{
		_ix = 1.f;
		_iy = 0.f;
		_iz = 0.f;
		_iw = 0.f;

		_jx = 0.f;
		_jy = 1.f;
		_jz = 0.f;
		_jw = 0.f;

		_kx = 0.f;
		_ky = 0.f;
		_kz = 1.f;
		_kw = 0.f;

		_tx = 0.f;
		_ty = 0.f;
		_tz = 0.f;
		_tw = 1.f;
	}

	~mat4() { /*DO NOTHING*/ }
	mat4& operator=(const mat4& copyFrom)
	{
		for (size_t index = 0; index < 16; ++index)
			m_value[index] = copyFrom.m_value[index];
		return *this;
	}
	mat4(const mat4& copyFrom) { *this = copyFrom; }
	mat4(const vec2& i_basis, const vec2& j_basis, const vec2& traslate={0, 0})
	{
		 _ix = i_basis.x;	_iy = i_basis.y;	_iz = 0;  _iw = 0;
		 _jx = j_basis.x;	_jy = j_basis.y;	_jz = 0;  _jw = 0;
		 _kx = 0;			_ky = 0;			_kz = 1;  _kw = 0;
		 _tx = traslate.x;  _ty = traslate.y;	_tz = 0;  _tw = 1;
	}
	mat4(const vec3& i_basis, const vec3& j_basis, const vec3& k_basis, const vec3& traslate={0, 0, 0})
	{
		_ix = i_basis.x;  _iy = i_basis.y;  _iz = i_basis.z;  _iw = 0;
		_jx = j_basis.x;  _jy = j_basis.y;  _jz = j_basis.z;  _jw = 0;
		_kx = k_basis.x;  _ky = k_basis.y;  _kz = k_basis.z;  _kw = 0;
		_tx = traslate.x;  _ty = traslate.y;  _tz = traslate.z;  _tw = 1;
	}
	mat4(const vec4& i_basis, const vec4& j_basis, const vec4& k_basis, const vec4& translate)
	{
		i = i_basis;
		j = j_basis;
		k = k_basis;
		t = translate;
	}
	mat4(const float values[/*16*/])
	{
		for (size_t index = 0; index < 16; ++index)
			m_value[index] = values[index];
	}

	float32& operator[](int index) {
		return m_value[index];
	}
	float32 operator[](int index) const {
		return m_value[index];
	}

	void transpose()
	{
		float m[16];
		for (size_t index = 0; index < 16; ++index) {
			m[index] = m_value[index];
		}
		m_value[1]  = m[4];
		m_value[2]  = m[8];
		m_value[3]  = m[12];
		m_value[4]  = m[1];
		m_value[6]  = m[9];
		m_value[7]  = m[13];
		m_value[8]  = m[2];
		m_value[9]  = m[6];
		m_value[11] = m[14];
		m_value[12] = m[3];
		m_value[13] = m[7];
		m_value[14] = m[11];
	}

	NODISCARD mat4 transposed() const
	{
		mat4 transposed(m_value);
		transposed.transpose();
		return transposed;
	}
	static const mat4 identity;
};
}
