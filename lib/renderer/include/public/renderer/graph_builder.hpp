#ifndef __VTX_RENDERER_GRAPH_BUILDER__
#define __VTX_RENDERER_GRAPH_BUILDER__

#include "descriptors.hpp"
#include <cstring>
#include <util/color/rgba.hpp>

namespace VTX::Renderer
{

	/**
	 * @brief Type to enum.
	 */
	template<typename T>
	constexpr E_TYPE uniformTypeOf();

	template<>
	constexpr E_TYPE uniformTypeOf<bool>()
	{
		return E_TYPE::BOOL;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<float>()
	{
		return E_TYPE::FLOAT;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<unsigned int>()
	{
		return E_TYPE::UINT;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<Vec2i>()
	{
		return E_TYPE::VEC2I;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<Vec2f>()
	{
		return E_TYPE::VEC2F;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<Vec3f>()
	{
		return E_TYPE::VEC3F;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<Vec4f>()
	{
		return E_TYPE::VEC4F;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<Mat3f>()
	{
		return E_TYPE::MAT3F;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<Mat4f>()
	{
		return E_TYPE::MAT4F;
	}
	template<>
	constexpr E_TYPE uniformTypeOf<Util::Color::Rgba>()
	{
		return E_TYPE::VEC4F;
	}

	/**
	 * @brief Create a uniform value.
	 */
	template<typename T>
	UniformValue makeUniform(
		const Key &									   p_name,
		const T &									   p_value,
		const std::optional<std::pair<double, double>> p_range = std::nullopt
	)
	{
		UniformValue u {};
		u.name = p_name;
		u.type = uniformTypeOf<T>();

		static_assert( sizeof( T ) <= u.data.size(), "UniformValue::data too small for T" );
		std::memcpy( u.data.data(), &p_value, sizeof( T ) );

		if constexpr ( std::is_floating_point_v<T> )
		{
			u.range = p_range;
		}

		return u;
	}

	template<typename T>
	UniformValue makeUniformArray(
		const Key &									   p_name,
		const T &									   p_value,
		const std::uint32_t							   p_count,
		const std::optional<std::pair<double, double>> p_range = std::nullopt
	)
	{
		UniformValue u = makeUniform<T>( p_name, p_value, p_range );
		u.arrayCount   = p_count;
		return u;
	}

	/**
	 * @brief Name of the default sampler used to avoid repetition.
	 */
	const Key DEFAULT_SAMPLER_NAME = "Default";

	/**
	 * @brief Forward declarations.
	 */
	struct ProgramBuilder;
	struct PassBuilder;

	/**
	 * @brief Graph language builder.
	 */
	struct GraphBuilder
	{
		Resources resources;
		PassList  passes;

		/**
		 * @brief Constructor.
		 */
		GraphBuilder();

		/**
		 * @brief texture().
		 */
		GraphBuilder & texture( const Key &, const E_FORMAT, const Size2D & = std::monostate {} );

		/**
		 * @brief texture().
		 */
		template<typename T>
		GraphBuilder & texture(
			const Key &			   p_name,
			const E_FORMAT		   p_format,
			const std::vector<T> & p_data,
			const Size2D &		   p_size = std::monostate {}
		)
		{
			Texture tex;
			tex.format = p_format;
			tex.size   = p_size;
			tex.data.resize( p_data.size() * sizeof( T ) );
			std::memcpy( tex.data.data(), p_data.data(), p_data.size() * sizeof( T ) );

			resources.textures.emplace( p_name, std::move( tex ) );
			return *this;
		}

		/**
		 * @brief sampler().
		 */
		GraphBuilder & sampler(
			const Key & p_name,
			const E_WRAPPING  = E_WRAPPING::CLAMP_TO_EDGE,
			const E_WRAPPING  = E_WRAPPING::CLAMP_TO_EDGE,
			const E_FILTERING = E_FILTERING::NEAREST,
			const E_FILTERING = E_FILTERING::NEAREST
		);

		/**
		 * @brief sampler().
		 */
		inline GraphBuilder & defaultSampler(
			const E_WRAPPING  p_wrapS	  = E_WRAPPING::CLAMP_TO_EDGE,
			const E_WRAPPING  p_wrapT	  = E_WRAPPING::CLAMP_TO_EDGE,
			const E_FILTERING p_minFilter = E_FILTERING::NEAREST,
			const E_FILTERING p_magFilter = E_FILTERING::NEAREST
		)
		{
			return sampler( DEFAULT_SAMPLER_NAME, p_wrapS, p_wrapT, p_minFilter, p_magFilter );
		}

		/**
		 * @brief vertexStream().
		 */
		GraphBuilder & vertexLayout( const Key &, const std::initializer_list<VertexAttribute> );

		/**
		 * @brief uniformBuffer().
		 */
		GraphBuilder & shaderBuffer(
			const Key &,
			const E_SHADER_BUFFER_KIND,
			const E_BUFFER_MUTABILITY,
			const E_BUFFER_ACCESS,
			const E_UPDATE_FREQUENCY,
			const uint32_t,
			const std::initializer_list<UniformValue> = {}
		);

		/**
		 * @brief dataBuffer().
		 */
		GraphBuilder & pipelineBuffer(
			const Key &					 p_name,
			const E_PIPELINE_BUFFER_KIND p_kind		 = E_PIPELINE_BUFFER_KIND::VERTEX,
			const E_UPDATE_FREQUENCY	 p_frequency = E_UPDATE_FREQUENCY::STATIC
		);

		/**
		 * @brief geometry().
		 */
		GraphBuilder & geometry(
			const Key & p_name,
			const Key & p_vertexStream,
			// const std::unordered_map<Key, Key> & p_overrides = {},
			const std::optional<Key> p_indexBuffer = std::nullopt
		);

		/**
		 * @brief pass().
		 */
		PassBuilder pass( const Key & );
	};

	/**
	 * @brief Program builder.
	 */
	struct ProgramBuilder
	{
		/**
		 * @brief Parent pass builder.
		 */
		PassBuilder & parent;

		/**
		 * @brief Program being built.
		 */
		Program & program;

		/**
		 * @brief Constructor.
		 */
		ProgramBuilder( PassBuilder &, Program & );

		/**
		 * @brief shaders().
		 */
		ProgramBuilder & shaders( std::initializer_list<FilePath> );
		ProgramBuilder & shadersDir( const FilePath & );

		/**
		 * @brief uniform().
		 */
		ProgramBuilder & uniform( const UniformValue & );

		/**
		 * @brief draw().
		 */
		ProgramBuilder & draw(
			const Key &		  p_geometry,
			const E_PRIMITIVE p_primitive	= E_PRIMITIVE::TRIANGLES,
			const uint32_t	  p_vertexCount = 0,
			const uint32_t	  p_indexCount	= 0
		);

		/**
		 * @brief uniform().
		 */
		template<typename T>
		ProgramBuilder & uniform(
			const Key &									   p_name,
			const T &									   p_value,
			const std::optional<std::pair<double, double>> p_range = std::nullopt
		)
		{
			program.uniforms.push_back( makeUniform( p_name, p_value, p_range ) );
			return *this;
		}

		/**
		 * @brief endProgram().
		 */
		PassBuilder & endProgram();
	};

	/**
	 * @brief Pass builder.
	 */
	struct PassBuilder
	{
		/**
		 * @brief Parent graph builder.
		 */
		GraphBuilder & graph;

		/**
		 * @brief Pass being built.
		 */
		Pass pass;

		/**
		 * @brief Constructor.
		 */
		PassBuilder( GraphBuilder &, const Key & );

		/**
		 * @brief in().
		 */
		// Generic binding.
		PassBuilder & in( const E_RESOURCE_TYPE, const Key &, const std::optional<Key> = std::nullopt );

		// Convenience overload: defaults to TEXTURE.
		PassBuilder & in( const Key &, const std::optional<Key> & = std::nullopt );

		/**
		 * @brief out().
		 */
		// Generic binding.
		PassBuilder & out( const E_RESOURCE_TYPE, const Key &, const std::optional<Key> = std::nullopt );

		// Convenience overload: defaults to TEXTURE.
		PassBuilder & out( const Key & );

		/**
		 * @brief settings().
		 */
		PassBuilder & settings( const std::initializer_list<Setting> );

		/**
		 * @brief program().
		 */
		ProgramBuilder program( const Key & );

		/**
		 * @brief callback().
		 */
		PassBuilder & callback( const RenderFunc );

		/**
		 * @brief endPass().
		 */
		GraphBuilder & endPass();
	};

} // namespace VTX::Renderer

#endif
