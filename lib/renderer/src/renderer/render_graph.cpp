#include "renderer/render_graph.hpp"
#include "renderer/graphics_config.hpp"
#include <unordered_set>
#include <util/constants.hpp>
#include <util/exceptions.hpp>
#include <util/math.hpp>

namespace VTX::Renderer
{
	const RenderQueue RenderGraph::build()
	{
		// Check if all inputs are produced before use.
		std::unordered_set<Key> produced;

		auto isExternalResource = [ this ]( const Key & name ) -> bool
		{
			if ( _resources.vertexStreams.contains( name ) )
			{
				return true;
			}
			if ( _resources.buffers.contains( name ) )
			{
				return true;
			}
			return false;
		};

		auto hasInputData = [ this ]( const Key & name ) -> bool
		{
			if ( _resources.textures.contains( name ) )
			{
				return not _resources.textures.at( name ).data.empty();
			}
			return false;
		};

		auto checkBinding = [ this ]( const ResourceBinding & res ) -> bool
		{
			if ( _resources.textures.contains( res.primary ) )
			{
				if ( res.secondary && not _resources.samplers.contains( *res.secondary ) )
				{
					return false;
				}
			}

			return true;
		};

		for ( const auto & passPtr : _passes )
		{
			const Pass & pass = *passPtr;

			for ( const auto & input : pass.inputs )
			{
				if ( not checkBinding( input ) )
				{
					throw GraphicException( "Texture '{}': incorrect binding", input.primary );
				}

				const bool external		  = isExternalResource( input.primary );
				const bool producedBefore = produced.contains( input.primary );
				const bool hasData		  = hasInputData( input.primary );

				// If not external, not produced before and has no data.
				if ( not external && not producedBefore && not hasData )
				{
					throw GraphicException( "Pass '{}' need not produced ressource '{}' ", pass.name, input.primary );
				}
			}

			// Set outputs as produced.
			for ( const auto & output : pass.outputs )
			{
				if ( not checkBinding( output ) )
				{
					throw GraphicException( "Texture '{}': incorrect binding", output.primary );
				}

				produced.insert( output.primary );
			}
		}

		// Build.
		// TODO: remove unused passes.
		RenderQueue queue;
		for ( auto & p : _passes )
		{
			queue.push_back( p.get() );
		}

		// Check empty.
		if ( queue.empty() )
		{
			throw GraphicException( "Render queue is empty" );
		}

		// Check last pass = 1 output.
		if ( queue.back()->outputs.size() != 1 )
		{
			throw GraphicException( "Last pass '{}' must have exactly one output", queue.back()->name );
		}

		// Print.
		std::string str = "Passes: ";
		for ( const Pass * const pass : queue )
		{
			str += pass->name + " -> ";
		}
		str += "Output";
		VTX_DEBUG( "{}", str );

		return queue;
	}

	void RenderGraph::add( const GraphBuilder & p_builder )
	{
		// Resources.
		for ( const auto & [ key, texture ] : p_builder.resources.textures )
		{
			auto [ it, inserted ] = _resources.textures.emplace( key, texture );
			assert( inserted );
		}
		for ( const auto & [ key, vertexStream ] : p_builder.resources.vertexStreams )
		{
			auto [ it, inserted ] = _resources.vertexStreams.emplace( key, vertexStream );
			assert( inserted );
		}
		for ( const auto & [ key, buffer ] : p_builder.resources.buffers )
		{
			auto [ it, inserted ] = _resources.buffers.emplace( key, buffer );
			assert( inserted );
		}
		for ( const auto & [ key, sampler ] : p_builder.resources.samplers )
		{
			auto [ it, inserted ] = _resources.samplers.emplace( key, sampler );
		}
		// Passes.
		for ( const auto & pass : p_builder.passes )
		{
			_passes.push_back( std::make_unique<Pass>( *pass ) );
		}
	}

	void RenderGraph::set( GraphBuilder && p_builder )
	{
		clear();
		_resources = std::move( p_builder.resources );
		_passes	   = std::move( p_builder.passes );
	}

	void RenderGraph::clear()
	{
		// Clear resources?
		//_resources = Resources {};
		_passes.clear();
	}

	void RenderGraph::createDefaultPipeline( const PipelineConfig & p_config )
	{
		GraphBuilder g;

		// Uniforms.
		g.buffer(
			"Camera",
			E_BUFFER_CLASS::UNIFORM_LIKE,
			E_BUFFER_ACCESS::READ,
			E_UPDATE_FREQUENCY::PER_FRAME,
			15,
			{ makeUniform( "MatrixView", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixProjection", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixViewInv", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixViewInvTrans", Mat4f( MAT4F_ID ) ),
			  makeUniform( "Position", Vec3f( VEC3F_ZERO ) ),
			  makeUniform( "ClipInfos", Vec4f( VEC4F_ZERO ) ),
			  makeUniform( "Resolution", Vec2i { 0, 0 } ),
			  makeUniform( "MousePosition", Vec2i { 0, 0 } ),
			  makeUniform( "IsPerspective", std::uint32_t( 1 ) ) }
		);

		g.buffer(
			"ColorLayout",
			E_BUFFER_CLASS::UNIFORM_LIKE,
			E_BUFFER_ACCESS::READ,
			E_UPDATE_FREQUENCY::STATIC,
			14,
			{ makeUniformArray( "Colors", Util::Color::Rgba {}, 256 ) }
		);

		g.buffer(
			"Models",
			E_BUFFER_CLASS::STRUCTURED,
			E_BUFFER_ACCESS::READ,
			E_UPDATE_FREQUENCY::PER_FRAME,
			13,
			{ makeUniform( "MatrixModelView", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixModelViewInv", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixNormal", Mat4f( MAT4F_ID ) ) }
		);

		g.buffer(
			"Representations",
			E_BUFFER_CLASS::STRUCTURED,
			E_BUFFER_ACCESS::READ,
			E_UPDATE_FREQUENCY::STATIC,
			12,
			{ makeUniform( "SphereRadiusFixed", 0.0f ),
			  makeUniform( "SphereRadiusAdd", 0.0f ),
			  makeUniform( "IsSphereRadiusFixed", std::uint32_t( 0 ) ),
			  makeUniform( "CylinderRadius", 0.0f ),
			  makeUniform( "CylinderColorBlending", std::uint32_t( 0 ) ),
			  makeUniform( "RibbonColorBlending", std::uint32_t( 0 ) ),
			  makeUniform( "SESProbeRadius", 0.0f ),
			  makeUniform( "SESMaxProbeNeighborNb", std::uint32_t( 0 ) ) }
		);

		// Vertex streams.
		g.vertexStream(
			"SpheresCylinders",
			{
				{ "Positions", E_TYPE::FLOAT, 3 },
				{ "Colors", E_TYPE::UBYTE, 1 },
				{ "Radii", E_TYPE::FLOAT, 1 },
				{ "Ids", E_TYPE::UINT, 1 },
				{ "Flags", E_TYPE::UBYTE, 1 },
				{ "Models", E_TYPE::USHORT, 1 },
				{ "Representations", E_TYPE::UBYTE, 1 },
			}
		);

		g.vertexStream(
			"Ribbons",
			{
				{ "Positions", E_TYPE::FLOAT, 4 },
				{ "Directions", E_TYPE::FLOAT, 3 },
				{ "Types", E_TYPE::UBYTE, 1 },
				{ "Colors", E_TYPE::UBYTE, 1 },
				{ "Ids", E_TYPE::UINT, 1 },
				{ "Flags", E_TYPE::UBYTE, 1 },
				{ "Models", E_TYPE::USHORT, 1 },
				{ "Representations", E_TYPE::UBYTE, 1 },
			}
		);

		g.vertexStream(
			"Triangles",
			{
				{ "Positions", E_TYPE::FLOAT, 3 },
				{ "Normales", E_TYPE::FLOAT, 3 },
				{ "Colors", E_TYPE::UBYTE, 1 },
				{ "Ids", E_TYPE::UINT, 1 },
				{ "Flags", E_TYPE::UBYTE, 1 },
				{ "Models", E_TYPE::USHORT, 1 },
				{ "Representations", E_TYPE::UBYTE, 1 },
			}
		);

		g.vertexStream(
			"Voxels",
			{
				{ "Mins", E_TYPE::FLOAT, 3 },
				{ "Maxs", E_TYPE::FLOAT, 3 },
			}
		);

		// Textures.
		g.texture( "Geometry", E_FORMAT::RGBA32UI )
			.texture( "Color", E_FORMAT::RGBA16F )
			.texture( "Picking", E_FORMAT::RG32UI )
			.texture( "DepthRaw", E_FORMAT::DEPTH_COMPONENT32F );

		g.texture( "Depth", E_FORMAT::R32F );

		// Used by shading pass even if SSAO disabled.
		// std::vector<float> emptyData( 1, 1.f );
		g.texture( "BlurX", E_FORMAT::R16F );
		g.texture( "Blur", E_FORMAT::R16F /*, emptyData */ );
		if ( p_config.enableSSAO )
		{
			constexpr size_t   noiseTextureSize = 64;
			std::vector<Vec3f> noiseData( noiseTextureSize * noiseTextureSize );
			std::generate(
				noiseData.begin(),
				noiseData.end(),
				[]
				{
					return Util::Math::normalize(
						Vec3f( Util::Math::randomFloat() * 2.f - 1.f, Util::Math::randomFloat() * 2.f - 1.f, 0.f )
					);
				}
			);
			g.texture( "SSAO", E_FORMAT::R8 )
				.texture( "Noise", E_FORMAT::RGB16F, noiseData, Size2DAbsolute { noiseTextureSize, noiseTextureSize } );
		}

		g.texture( "Shaded", E_FORMAT::RGBA16F );

		if ( p_config.enableOutline )
		{
			g.texture( "Outline", E_FORMAT::RGBA16F );
		}

		if ( p_config.enableSelection )
		{
			g.texture( "Selection", E_FORMAT::RGBA16F );
		}

		g.texture( "FXAA", E_FORMAT::RGBA16F );

		// Samplers.
		g.sampler(
			"NearestClamp",
			E_WRAPPING::CLAMP_TO_EDGE,
			E_WRAPPING::CLAMP_TO_EDGE,
			E_FILTERING::NEAREST,
			E_FILTERING::NEAREST
		);

		g.sampler(
			"NearestRepeat", E_WRAPPING::REPEAT, E_WRAPPING::REPEAT, E_FILTERING::NEAREST, E_FILTERING::NEAREST
		);

		g.sampler(
			"LinearClamp",
			E_WRAPPING::CLAMP_TO_EDGE,
			E_WRAPPING::CLAMP_TO_EDGE,
			E_FILTERING::LINEAR,
			E_FILTERING::LINEAR
		);

		// Passes.
		// Geometric.
		g.pass( "Geometric" )
			.in( "SpheresCylinders" )
			.in( "Ribbons" )
			.in( "Triangles" )
			.in( "Voxels" )
			.out( "Geometry" )
			.out( "Color" )
			.out( "Picking" )
			.out( "DepthRaw" )
			.program( "Sphere" )
			.shaders( { FilePath( "sphere" ) } )
			.draw( "SpheresCylinders", E_PRIMITIVE::POINTS )
			.endProgram()
			.program( "Cylinder" )
			.shaders( { FilePath( "cylinder" ) } )
			.draw( "SpheresCylinders", E_PRIMITIVE::LINES, true )
			.endProgram()
			.program( "Ribbon" )
			.shaders( { FilePath( "ribbon" ) } )
			.draw( "Ribbons", E_PRIMITIVE::PATCHES, true )
			.endProgram()
			.program( "Voxel" )
			.shaders( { FilePath( "voxel" ) } )
			.draw( "Voxels", E_PRIMITIVE::POINTS )
			.endProgram()
			.endPass();

		// Linearize depth.
		g.pass( "LinearizeDepth" )
			.in( "DepthRaw" )
			.out( "Depth" )
			.program( "LinearizeDepth" )
			.shaders( { FilePath( "default.vert" ), FilePath( "linearize_depth.frag" ) } )
			.endProgram()
			.endPass();

		if ( p_config.enableSSAO )
		{
			// SSAO.
			g.pass( "SSAO" )
				.in( "Geometry" )
				.in( "Noise" )
				.in( "Depth" )
				.out( "SSAO", "NearestRepeat" )
				.program( "SSAO" )
				.shaders( { FilePath( "default.vert" ), FilePath( "ssao.frag" ) } )
				.uniform( "Intensity", SSAO_INTENSITY_DEFAULT, std::pair { SSAO_INTENSITY_MIN, SSAO_INTENSITY_MAX } )
				.endProgram()
				.endPass();

			// BlurX.
			g.pass( "BlurX" )
				.in( "SSAO" )
				.in( "Depth" )
				.out( "BlurX", "NearestRepeat" )
				.program( "Blur" )
				.shaders( { FilePath( "default.vert" ), FilePath( "blur.frag" ) } )
				.uniform( "Direction", Vec2i( 1, 0 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.endProgram()
				.endPass();
			// BlurY.
			g.pass( "BlurY" )
				.in( "BlurX" )
				.in( "Depth" )
				.out( "Blur", "NearestRepeat" )
				.program( "Blur" )
				.shaders( { FilePath( "default.vert" ), FilePath( "blur.frag" ) } )
				.uniform( "Direction", Vec2i( 0, 1 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.endProgram()
				.endPass();
		}

		// Shading.
		g.pass( "Shading" )
			.in( "Geometry" )
			.in( "Color" )
			.in( "Blur", "NearestRepeat" )
			.out( "Shaded" )
			.program( "Shading" )
			.shaders( { FilePath( "default.vert" ), FilePath( "shading.frag" ) } )
			.uniform( "BackgroundColor", COLOR_BACKGROUND_DEFAULT )
			.uniform( "LightColor", COLOR_LIGHT_DEFAULT )
			.uniform( "FogColor", COLOR_FOG_DEFAULT )
			.uniform(
				"Mode",
				static_cast<uint>( SHADING_MODE_DEFAULT ),
				std::pair { static_cast<uint>( E_SHADING::DIFFUSE ), static_cast<uint>( E_SHADING::COUNT ) - 1.0 }
			)
			.uniform(
				"SpecularFactor", SPECULAR_FACTOR_DEFAULT, std::pair { SPECULAR_FACTOR_MIN, SPECULAR_FACTOR_MAX }
			)
			.uniform( "Shininess", SHININESS_DEFAULT, std::pair { SHININESS_MIN, SHININESS_MAX } )
			.uniform( "ToonSteps", TOON_STEPS_DEFAULT, std::pair { TOON_STEPS_MIN, TOON_STEPS_MAX } )
			.uniform( "FogNear", FOG_NEAR_DEFAULT, std::pair { FOG_NEAR_MIN, FOG_NEAR_MAX } )
			.uniform( "FogFar", FOG_FAR_DEFAULT, std::pair { FOG_FAR_MIN, FOG_FAR_MAX } )
			.uniform( "FogDensity", FOG_DENSITY_DEFAULT, std::pair { FOG_DENSITY_MIN, FOG_DENSITY_MAX } )
			.endProgram()
			.endPass();

		// Outline.
		if ( p_config.enableOutline )
		{
			g.pass( "Outline" )
				.in( "Shaded" )
				.in( "Depth" )
				.out( "Outline" )
				.program( "Outline" )
				.shaders( { FilePath( "default.vert" ), FilePath( "outline.frag" ) } )
				.uniform( "Color", COLOR_WHITE )
				.uniform(
					"Sensitivity",
					OUTLINE_SENSITIVITY_DEFAULT,
					std::pair { OUTLINE_SENSITIVITY_MIN, OUTLINE_SENSITIVITY_MAX }
				)
				.uniform(
					"Thickness", OUTLINE_THICKNESS_DEFAULT, std::pair { OUTLINE_THICKNESS_MIN, OUTLINE_THICKNESS_MAX }
				)
				.endProgram()
				.endPass();
		}

		// Selection.
		if ( p_config.enableSelection )
		{
			g.pass( "Selection" )
				.in( "Geometry" )
				.in( "Shaded" )
				.in( "Depth" )
				.out( "Selection" )
				.program( "Selection" )
				.shaders( { FilePath( "default.vert" ), FilePath( "selection.frag" ) } )
				.uniform( "Color", COLOR_SELECTION_DEFAULT )
				.endProgram()
				.endPass();
		}

		// FXAA.
		g.pass( "FXAA" )
			.in( "Shaded" )
			.out( "FXAA" )
			.program( "FXAA" )
			.shaders( { FilePath( "default.vert" ), FilePath( "fxaa.frag" ) } )
			.endProgram()
			.endPass();

		// Pixelize
		/*
		g.pass( "Pixelize" )
			.in( "FXAA" )
			.out( "Pixelize" )
			.program( "Pixelize" )
			.shaders( { FilePath( "default.vert" ), FilePath( "pixelize.frag" ) } )
			.uniform( "Size", static_cast<std::uint32_t>( 5 ), std::pair { 1.0, 15.0 } )
			.uniform( "Background", true )
			.endProgram()
			.endPass();
			*/

		// CRT
		/*
		g.pass( "CRT" )
			.in( "Pixelize" )
			.out( "CRT" )
			.program( "CRT" )
			.shaders( { FilePath( "default.vert" ), FilePath( "crt.frag" ) } )
			.uniform( "Curvature", Vec2f( 3.f, 3.f ) )
			.uniform( "Ratio", 0.25f, std::pair { 0.1, 1.0 } )
			.uniform( "GraninessX", 0.5f, std::pair { 0.0, 5.0 } )
			.uniform( "GraninessY", 0.5f, std::pair { 0.0, 5.0 } )
			.uniform( "VignetteRoundness", 100.f, std::pair { 1.0, 1000.0 } )
			.uniform( "VignetteIntensity", 0.5f, std::pair { 0.0, 5.0 } )
			.uniform( "Brightness", 1.2f, std::pair { 1.0, 10.0 } )
			.endProgram()
			.endPass();
			*/

		// Chromatic aberration
		/*
		g.pass( "ChromaticAberration" )
			.in( "CRT" )
			.out( "ChromaticAberration" )
			.program( "ChromaticAberration" )
			.shaders( { FilePath( "default.vert" ), FilePath( "chromatic_aberration.frag" ) } )
			.uniform( "Red", 0.009f, std::pair { -0.05, 0.05 } )
			.uniform( "Green", 0.006f, std::pair { -0.05, 0.05 } )
			.uniform( "Blue", -0.006f, std::pair { -0.05, 0.05 } )
			.endProgram()
			.endPass();
			*/

		// Colorize
		/*
		g.pass( "Colorize" )
			.in( "ChromaticAberration" )
			.out( "Colorize" )
			.program( "Colorize" )
			.shaders( { FilePath( "default.vert" ), FilePath( "colorize.frag" ) } )
			.uniform( "Color", COLOR_YELLOW )
			.endProgram()
			.endPass();
			*/

		// Debug
		/*
		g.pass( "Debug" )
			.in( "Colorize" )
			.out( "Debug" )
			.program( "Debug" )
			.shaders( { FilePath( "default.vert" ), FilePath( "debug.frag" ) } )
			.uniform( "Color", COLOR_YELLOW )
			.uniform( "Color2", COLOR_BLUE )
			.uniform( "Test", 5646.0f )
			.uniform( "Factor", 5.0f, std::pair { 0.0, 10.0 } )
			.endProgram()
			.endPass();
			*/

		set( std::move( g ) );
	}

} // namespace VTX::Renderer
