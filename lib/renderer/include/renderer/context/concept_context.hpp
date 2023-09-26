#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include <concepts>
#include <functional>
#include <util/types.hpp>
#include <variant>

namespace VTX::Renderer::Context
{

	enum struct E_FORMAT
	{
		RGBA16F,
		RGBA32UI,
		RGBA32F,
		RG32UI,
		R16F,
		R32F,
		DEPTH_COMPONENT32F
	};

	enum struct E_HANDLE_TYPE
	{
		ATTACHMENT,
		STORAGE,
		// UNIFORM,
		PROGRAM,
		// ...?
	};

	enum struct E_TYPE
	{
		UINT,
		INT,
		FLOAT,
		DOUBLE,
		VEC2UI,
		VEC3UI,
		VEC4uI,
		VEC2I,
		VEC3I,
		VEC4I,
		VEC2F,
		VEC3F,
		VEC4F,
		MAT3F,
		MAT4F,
	};

	enum struct E_ACCESS
	{
		READ,
		WRITE,
		READ_WRITE,
	};

	enum struct E_WRAPPING
	{
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRROR_CLAMP_TO_EDGE,
	};

	enum struct E_FILTERING
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR,
	};

	// Base context.
	// template<typename F>
	struct BaseContext
	{
		size_t	 width;
		size_t	 height;
		FilePath shaderPath;
	};

	// Descriptors.
	struct DescAttachment
	{
		E_FORMAT	format		 = E_FORMAT::RGBA16F;
		E_WRAPPING	wrappingS	 = E_WRAPPING::CLAMP_TO_EDGE;
		E_WRAPPING	wrappingT	 = E_WRAPPING::CLAMP_TO_EDGE;
		E_FILTERING filteringMin = E_FILTERING::NEAREST;
		E_FILTERING filteringMag = E_FILTERING::NEAREST;
	};

	struct DescStorage
	{
	};

	// 	struct DescUniform
	// 	{
	// 	};

	struct DescProgram
	{
		std::string									  name;
		std::variant<FilePath, std::vector<FilePath>> shaders;
		std::string									  toInject;
		std::string									  suffix;
	};

	using Handle = uint;
	using Desc	 = std::variant<DescAttachment, DescStorage, DescProgram>;
	using DescIO = std::variant<DescAttachment, DescStorage>;

	template<typename C>
	concept Concept = requires( C					   p_context,
								const DescAttachment & p_descAttachment,
								const DescStorage &	   p_descStorage,
								const DescProgram &	   p_descProgram,
								const size_t		   p_width,
								const size_t		   p_height,
								Handle &			   p_handle ) {
		{
			p_context.clear()
		} -> std::same_as<void>;
		{
			p_context.resize( p_width, p_height )
		} -> std::same_as<void>;
		{
			p_context.create( p_handle, p_descAttachment )
		} -> std::same_as<void>;
		{
			p_context.resize( p_handle, p_descAttachment )
		} -> std::same_as<void>;
		{
			p_context.destroy( p_handle, p_descAttachment )
		} -> std::same_as<void>;
		{
			p_context.create( p_handle, p_descStorage )
		} -> std::same_as<void>;
		{
			p_context.create( p_handle, p_descProgram )
		} -> std::same_as<void>;
		{
			p_context.destroy( p_handle, p_descProgram )
		} -> std::same_as<void>;
	};

} // namespace VTX::Renderer::Context

#endif
