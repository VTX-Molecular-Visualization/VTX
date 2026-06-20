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
	constexpr Desc::E_TYPE uniformTypeOf();

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<bool>()
	{ return Desc::E_TYPE::BOOL; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<float>()
	{ return Desc::E_TYPE::FLOAT; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<unsigned int>()
	{ return Desc::E_TYPE::UINT; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<Vec2i>()
	{ return Desc::E_TYPE::VEC2I; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<Vec2f>()
	{ return Desc::E_TYPE::VEC2F; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<Vec3f>()
	{ return Desc::E_TYPE::VEC3F; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<Vec4f>()
	{ return Desc::E_TYPE::VEC4F; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<Mat3f>()
	{ return Desc::E_TYPE::MAT3F; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<Mat4f>()
	{ return Desc::E_TYPE::MAT4F; }

	template<>
	constexpr Desc::E_TYPE uniformTypeOf<Util::Color::Rgba>()
	{ return Desc::E_TYPE::VEC4F; }

	/**
	 * @brief Create a uniform value.
	 */
	template<typename T>
	Desc::UniformValue makeUniform(
		const Desc::Key &							   p_name,
		const T &									   p_value,
		const std::optional<std::pair<double, double>> p_range = std::nullopt
	)
	{
		Desc::UniformValue u {};
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
	Desc::UniformValue makeUniformArray(
		const Desc::Key &							   p_name,
		const T &									   p_value,
		const std::uint32_t							   p_count,
		const std::optional<std::pair<double, double>> p_range = std::nullopt
	)
	{
		Desc::UniformValue u = makeUniform<T>( p_name, p_value, p_range );
		u.arrayCount		 = p_count;
		return u;
	}

	/**
	 * @brief Name of the default sampler used to avoid repetition.
	 */
	const Desc::Key DEFAULT_SAMPLER_NAME = "Default";

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
		Desc::Resources resources;
		Desc::PassList	passes;

		/**
		 * @brief Constructor.
		 */
		GraphBuilder();

		/**
		 * @brief texture().
		 */
		GraphBuilder & texture(
			const Desc::Key &,
			const Desc::E_FORMAT,
			const Desc::Size2D &		 = std::monostate {},
			const Desc::E_TEXTURE_TARGET = Desc::E_TEXTURE_TARGET::TEXTURE_2D,
			const bool					 = false
		);

		/**
		 * @brief texture().
		 */
		template<typename T>
		GraphBuilder & texture(
			const Desc::Key &	   p_name,
			const Desc::E_FORMAT   p_format,
			const std::vector<T> & p_data,
			const Desc::Size2D &   p_size = std::monostate {}
		)
		{
			Desc::Texture tex;
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
			const Desc::Key & p_name,
			const Desc::E_WRAPPING	= Desc::E_WRAPPING::CLAMP_TO_EDGE,
			const Desc::E_WRAPPING	= Desc::E_WRAPPING::CLAMP_TO_EDGE,
			const Desc::E_FILTERING = Desc::E_FILTERING::NEAREST,
			const Desc::E_FILTERING = Desc::E_FILTERING::NEAREST
		);

		/**
		 * @brief sampler().
		 */
		inline GraphBuilder & defaultSampler(
			const Desc::E_WRAPPING	p_wrapS		= Desc::E_WRAPPING::CLAMP_TO_EDGE,
			const Desc::E_WRAPPING	p_wrapT		= Desc::E_WRAPPING::CLAMP_TO_EDGE,
			const Desc::E_FILTERING p_minFilter = Desc::E_FILTERING::NEAREST,
			const Desc::E_FILTERING p_magFilter = Desc::E_FILTERING::NEAREST
		)
		{ return sampler( DEFAULT_SAMPLER_NAME, p_wrapS, p_wrapT, p_minFilter, p_magFilter ); }

		/**
		 * @brief vertexStream().
		 */
		GraphBuilder & vertexLayout( const Desc::Key &, const std::initializer_list<Desc::VertexAttribute> );

		/**
		 * @brief vertexStream().
		 */
		GraphBuilder & vertexLayout( const Desc::Key &, const Desc::VertexLayout & );

		/**
		 * @brief buffer().
		 */
		GraphBuilder & buffer(
			const Desc::Key &,
			const Desc::E_BUFFER_USAGE,
			const Desc::E_UPDATE_FREQUENCY					= Desc::E_UPDATE_FREQUENCY::STATIC,
			const Desc::E_BUFFER_ALLOCATION					= Desc::E_BUFFER_ALLOCATION::SINGLE,
			const Desc::E_BUFFER_MUTABILITY					= Desc::E_BUFFER_MUTABILITY::MUTABLE,
			const Desc::E_BUFFER_ACCESS						= Desc::E_BUFFER_ACCESS::NONE,
			const std::optional<uint32_t>					= std::nullopt,
			const std::initializer_list<Desc::UniformValue> = {}
		);

		/**
		 * @brief geometry().
		 */
		GraphBuilder & geometry( const Desc::Key &, const Desc::Geometry & );

		/**
		 * @brief geometry().
		 */
		GraphBuilder & geometry(
			const Desc::Key &,
			const Desc::Key &,
			const std::optional<Desc::Key> = {},
			const std::optional<Desc::Key> = {}
		);

		/**
		 * @brief pass().
		 */
		PassBuilder pass(
			const Desc::Key &,
			const Desc::E_PASS_TYPE		 = Desc::E_PASS_TYPE::GRAPHICS,
			const Desc::E_PASS_EXECUTION = Desc::E_PASS_EXECUTION::EVERY_FRAME
		);

		PassBuilder computePass(
			const Desc::Key &,
			const Desc::E_PASS_EXECUTION = Desc::E_PASS_EXECUTION::EVERY_FRAME
		);

		PassBuilder externalPass( const Desc::Key &, const Desc::E_PASS_EXECUTION = Desc::E_PASS_EXECUTION::ON_DIRTY );
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
		Desc::Program & program;

		/**
		 * @brief Constructor.
		 */
		ProgramBuilder( PassBuilder &, Desc::Program & );

		/**
		 * @brief shaders().
		 */
		ProgramBuilder & shaders( std::initializer_list<FilePath> );
		ProgramBuilder & shadersDir( const FilePath & );

		/**
		 * @brief uniform().
		 */
		ProgramBuilder & uniform( const Desc::UniformValue & );

		/**
		 * @brief draw().
		 */
		ProgramBuilder & draw( const Desc::Key &, const Desc::E_PRIMITIVE );
		ProgramBuilder & draw( const Desc::Key &, const Desc::E_PRIMITIVE, const Desc::DrawCall::RangesVariant & );

		/**
		 * @brief dispatch().
		 */
		ProgramBuilder & dispatch( const Desc::Dispatch & );
		ProgramBuilder & dispatch(
			const uint32_t,
			const uint32_t				 = 1,
			const uint32_t				 = 1,
			const Desc::E_MEMORY_BARRIER = Desc::E_MEMORY_BARRIER::NONE
		);
		ProgramBuilder & dispatchIndirect(
			const Desc::Key &,
			const uint32_t				 = 0,
			const Desc::E_MEMORY_BARRIER = Desc::E_MEMORY_BARRIER::NONE
		);

		/**
		 * @brief uniform().
		 */
		template<typename T>
		ProgramBuilder & uniform(
			const Desc::Key &							   p_name,
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
		Desc::Pass pass;

		/**
		 * @brief Constructor.
		 */
		PassBuilder( GraphBuilder &, const Desc::Key &, const Desc::E_PASS_TYPE, const Desc::E_PASS_EXECUTION );

		PassBuilder & type( const Desc::E_PASS_TYPE );
		PassBuilder & execution( const Desc::E_PASS_EXECUTION );

		/**
		 * @brief in().
		 */
		// Generic binding.
		PassBuilder & in(
			const Desc::E_RESOURCE_TYPE,
			const Desc::Key &,
			const std::optional<Desc::Key> = std::nullopt
		);

		// Convenience overload: defaults to TEXTURE.
		PassBuilder & in( const Desc::Key &, const std::optional<Desc::Key> & = std::nullopt );

		/**
		 * @brief out().
		 */
		// Generic binding.
		PassBuilder & out(
			const Desc::E_RESOURCE_TYPE,
			const Desc::Key &,
			const std::optional<Desc::Key> = std::nullopt
		);

		// Convenience overload: defaults to TEXTURE.
		PassBuilder & out( const Desc::Key & );

		/**
		 * @brief settings().
		 */
		PassBuilder & settings( const std::initializer_list<Desc::E_SETTING> );

		/**
		 * @brief program().
		 */
		ProgramBuilder program( const Desc::Key & );

		/**
		 * @brief endPass().
		 */
		GraphBuilder & endPass();
	};

} // namespace VTX::Renderer

#endif
