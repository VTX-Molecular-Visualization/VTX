#ifndef __VTX_RENDERER_GRAPH_BUILDER__
#define __VTX_RENDERER_GRAPH_BUILDER__

#include "descriptors.hpp"
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
		GraphBuilder & texture( const Key & p_name, const E_FORMAT p_format )
		{
			resources.textures[ p_name ] = Texture { p_format };
			return *this;
		}

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
		GraphBuilder & vertexStream( const Key & p_name, const std::initializer_list<VertexAttribute> p_attributes )
		{
			VertexLayout layout;
			layout.attributes.assign( p_attributes.begin(), p_attributes.end() );
			resources.vertexStreams[ p_name ] = std::move( layout );
			return *this;
		}

		/**
		 * @brief uniformBuffer().
		 */
		GraphBuilder & uniformBuffer(
			const Key &								  p_name,
			const std::uint32_t						  p_binding,
			const std::initializer_list<UniformValue> p_values = {}
		)
		{
			UniformBuffer desc;
			desc.name	 = p_name;
			desc.binding = p_binding;
			desc.values.assign( p_values.begin(), p_values.end() );
			resources.uniformBuffers[ p_name ] = std::move( desc );
			return *this;
		}

		PassBuilder pass( const Key & p_name );
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
		ProgramBuilder( PassBuilder & p_p, Program & p_prog ) : parent( p_p ), program( p_prog ) {}

		ProgramBuilder & shaders( std::initializer_list<FilePath> p_files )
		{
			program.shaders.assign( p_files.begin(), p_files.end() );
			return *this;
		}

		/**
		 * @brief draw().
		 */
		ProgramBuilder & draw(
			const Key &		  p_vertexStream,
			const E_PRIMITIVE p_primitive,
			const bool		  p_useIndices = false
		)
		{
			DrawCall dc;
			dc.vertexStream	 = p_vertexStream;
			dc.primitive	 = p_primitive;
			dc.useIndices	 = p_useIndices;
			program.drawCall = dc;
			return *this;
		}

		/**
		 * @brief uniform().
		 */
		ProgramBuilder & uniform( const UniformValue & p_u )
		{
			program.uniforms.push_back( p_u );
			return *this;
		}

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

		PassBuilder( GraphBuilder & p_g, const Key & p_name ) : graph( p_g ) { pass.name = p_name; }

		/**
		 * @brief in().
		 */
		PassBuilder & in( const Key & p_resourceName )
		{
			pass.inputs.push_back( p_resourceName );
			return *this;
		}

		/**
		 * @brief out().
		 */
		PassBuilder & out( const Key & p_resourceName )
		{
			pass.outputs.push_back( p_resourceName );
			return *this;
		}

		/**
		 * @brief program().
		 */
		ProgramBuilder program( const Key & p_name )
		{
			pass.programs.emplace_back();
			Program & prog = pass.programs.back();
			prog.name	   = p_name;
			return ProgramBuilder( *this, prog );
		}

		/**
		 * @brief callback().
		 */
		PassBuilder & callback( const RenderFunc p_func )
		{
			pass.customCallback = std::move( p_func );
			return *this;
		}

		/**
		 * @brief endPass().
		 */
		GraphBuilder & endPass()
		{
			graph.passes.emplace_back( std::make_unique<Pass>( std::move( pass ) ) );
			return graph;
		}
	};

	/**
	 * @brief Forward declared implementations.
	 */
	inline PassBuilder	 GraphBuilder::pass( const Key & p_name ) { return PassBuilder( *this, p_name ); }
	inline PassBuilder & ProgramBuilder::endProgram() { return parent; }

} // namespace VTX::Renderer

#endif
