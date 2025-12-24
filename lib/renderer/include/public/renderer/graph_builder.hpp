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
		return E_TYPE::COLOR4;
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
		 * @brief texture().
		 */
		GraphBuilder & texture( const Key &, const E_FORMAT );

		/**
		 * @brief texture().
		 */
		template<typename T>
		GraphBuilder & texture( const Key & p_name, const E_FORMAT p_format, const std::vector<T> & p_data )
		{
			Texture tex;
			tex.format = p_format;
			tex.data.resize( p_data.size() * sizeof( T ) );
			std::memcpy( tex.data.data(), p_data.data(), p_data.size() * sizeof( T ) );

			resources.textures.emplace( p_name, std::move( tex ) );
			return *this;
		}

		/**
		 * @brief vertexStream().
		 */
		GraphBuilder & vertexStream( const Key &, const std::initializer_list<VertexAttribute> );

		/**
		 * @brief uniformBuffer().
		 */
		GraphBuilder & buffer(
			const Key &,
			const E_BUFFER_CLASS,
			const E_BUFFER_ACCESS,
			const E_UPDATE_FREQUENCY,
			const uint32_t,
			const std::initializer_list<UniformValue> = {}
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

		/**
		 * @brief draw().
		 */
		ProgramBuilder & draw( const Key &, const E_PRIMITIVE, const bool = false );

		/**
		 * @brief uniform().
		 */
		ProgramBuilder & uniform( const UniformValue & );

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
		PassBuilder & in( const Key & );

		/**
		 * @brief out().
		 */
		PassBuilder & out( const Key & );

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
