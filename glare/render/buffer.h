#pragma once
#include "glare/core/common.h"
#include "glare/render/common.h"
#include <unordered_map>
#include <typeindex>

namespace glare
{
struct super_vertex;
class renderer;
enum e_hlsl_type : uint32
{
	HLSL_NULL = 0,
	HLSL_FLOAT = sizeof(float32),
	HLSL_FLOAT2 = 2 * sizeof(float32),
	HLSL_FLOAT3 = 3 * sizeof(float32),
	HLSL_FLOAT4 = 4 * sizeof(float32)
};
using super_vertex_copier = void(*) (void* dst, const super_vertex* src, size_t count);

inline size_t get_size_of_hlsl_type(e_hlsl_type t)
{
	return static_cast<size_t>(t);
}

class buffer_layout
{
public:
	struct attribute
	{
	public:
		string name;
		e_hlsl_type type = HLSL_NULL;
		size_t offset = 0;
		
		attribute() = default;
		attribute(string name, e_hlsl_type type, size_t offset)
			: name(std::move(name)), type(type), offset(offset)
		{}

		NODISCARD bool is_null() const { return type == HLSL_NULL; }

		static attribute END();
	};
public:
	static std::unordered_map<std::type_index, const buffer_layout*> s_cached_layout;
	static const buffer_layout* acquire_layout(const attribute attr[], size_t stride, super_vertex_copier copy_func);

	template<typename T>
	static const buffer_layout* acquire_layout_of()
	{
		std::type_index key(typeid(T));
		const auto found = s_cached_layout.find(key);
		if (found == std::cend(s_cached_layout)) {
			const buffer_layout* created = acquire_layout(T::_s_attr, sizeof(T), T::_copy_from_super_vertex);
			s_cached_layout.emplace(std::make_pair(key, created));
			return created;
		}
		return found->second;
	}

public:
	buffer_layout() = default;
	buffer_layout(const attribute attr[]);
	const attribute& operator[] (size_t index) const { return m_attributes[index]; }
	NODISCARD size_t get_attributes_count() const { return m_attributes.size(); }
	NODISCARD size_t get_stride() const { return m_stride; }
	void copy_from_super_vertex(void* dst, const super_vertex* src, size_t count) const
	{
		m_copy_func(dst, src, count);
	}
public:
	std::vector<attribute> m_attributes;
	size_t m_stride;
	super_vertex_copier m_copy_func = nullptr;
};

class render_buffer
{
public:
	render_buffer(renderer* r);
	virtual ~render_buffer();

	NODISCARD size_t get_buffer_size() const	{ return m_buffer_size; }
	NODISCARD bool is_immutable() const			{ return m_memory_usage == GPU_MEMORY_IMMUTABLE; }
	NODISCARD bool is_dynamic() const			{ return m_memory_usage == GPU_MEMORY_DYNAMIC; }
	NODISCARD dx_buffer* get_buffer_handle() const { return m_handle; }

	bool create(const void* data, size_t buffer_size, size_t element_size, e_render_buffer_usage buffer_usage, e_gpu_memory_usage memory_usage);
	virtual bool buffer(const void* data, size_t size);

	renderer*				m_renderer		= nullptr;
	e_render_buffer_usage	m_buffer_usage	= RENDER_BUFFER_CONSTANT;
	e_gpu_memory_usage		m_memory_usage	= GPU_MEMORY_DYNAMIC;
	size_t					m_buffer_size	= 0;
	size_t					m_element_size	= 0;
	dx_buffer*				m_handle		= nullptr;
};

class constant_buffer : public render_buffer
{
public:
	constant_buffer(renderer* r);
	virtual ~constant_buffer() override = default;
	virtual bool buffer(const void* data, size_t size) override;
};

class vertex_buffer : public render_buffer
{
public:
	vertex_buffer(renderer* r);
	virtual ~vertex_buffer() override = default;
	virtual bool buffer(const void* data, size_t count) override;

	bool create_immutable_buffer(const void* data, size_t vertex_count, const buffer_layout* layout);
	void set_buffer_layout(const buffer_layout* layout) { m_buffer_layout = layout; }

	NODISCARD const buffer_layout* get_buffer_layout() const {return m_buffer_layout;}
	NODISCARD size_t get_count() const { return m_count; }
	
public:
	size_t m_count = 0;
	const buffer_layout* m_buffer_layout = nullptr;
};

class index_buffer : public render_buffer
{
public:
#if defined(GLARE_INDEX_T_UINT32)
	using index_t = uint32;
	static inline constexpr dx_format format = DXGI_FORMAT_R32_UINT;
#endif
#if defined(GLARE_INDEX_T_UINT16)
	using index_t = uint16;
	static inline constexpr dx_format format = DXGI_FORMAT_R16_UINT;
#endif
public:
	index_buffer(renderer* r);

	virtual ~index_buffer() override = default;
	virtual bool buffer(const void* data, size_t count) override;

	bool create_immutable_buffer(const index_t* data, size_t count);
	NODISCARD size_t get_count() const { return m_count; }

public:
	size_t m_count = 0;
};

};