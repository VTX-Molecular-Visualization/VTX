#ifndef __VTX_RENDERER_CONTEXT_CONCEPT__
#define __VTX_RENDERER_CONTEXT_CONCEPT__

#include <concepts>
#include <util/types.hpp>
#include <variant>

namespace VTX::Renderer::Context
{
	enum struct E_FORMAT
	{
		RGBA16F,
		RGBA32UI,
		R32F
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

	// Descriptors.
	struct DescAttachment
	{
		E_FORMAT format;
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

	template<typename T>
	using ElementOrVector = std::conditional<std::is_same_v<T, E_TYPE>, T, std::vector<T>>;

	using DescIO = union
	{
		DescAttachment attachment;
		DescStorage	   storage;
	};

	// using Id = size_t;

	template<typename C>
	concept Concept = requires( C					   p_context,
								const DescAttachment & p_descTexture,
								const DescStorage &	   p_descStorage,
								const DescProgram &	   p_descProgram ) {
		{
			p_context.add( p_descTexture )
		} -> std::same_as<void>;
		{
			p_context.add( p_descStorage )
		} -> std::same_as<void>;
		{
			p_context.add( p_descProgram )
		} -> std::same_as<void>;
	};

} // namespace VTX::Renderer::Context

#endif
