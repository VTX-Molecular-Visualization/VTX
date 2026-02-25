#include "renderer/render_graph.hpp"
#include "renderer/graphics_config.hpp"
#include <unordered_set>
#include <util/constants.hpp>
#include <util/exceptions.hpp>
#include <util/math.hpp>

namespace VTX::Renderer
{
	const Desc::RenderQueue RenderGraph::build()
	{
		using namespace Desc;

		// Check if all inputs are produced before use.
		std::unordered_set<Key> produced;

		auto isExternalResource = [ this ]( const Key & name ) -> bool
		{
			if ( _resources.geometries.contains( name ) )
			{
				return true;
			}
			if ( _resources.shaderBuffers.contains( name ) )
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
		Desc::RenderQueue queue;
		for ( auto & p : _passes )
		{
			queue.push_back( p.get() );
		}

		// Check empty.
		if ( queue.empty() )
		{
			return queue;
		}

		// Check last pass = no output (going to fbo).
		if ( queue.back()->outputs.size() > 0 )
		{
			// throw GraphicException( "Last pass '{}' must have exactly one output", queue.back()->name );
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
		for ( const auto & [ key, buffer ] : p_builder.resources.shaderBuffers )
		{
			auto [ it, inserted ] = _resources.shaderBuffers.emplace( key, buffer );
			assert( inserted );
		}
		for ( const auto & [ key, buffer ] : p_builder.resources.pipelineBuffers )
		{
			auto [ it, inserted ] = _resources.pipelineBuffers.emplace( key, buffer );
			assert( inserted );
		}
		for ( const auto & [ key, sampler ] : p_builder.resources.samplers )
		{
			auto [ it, inserted ] = _resources.samplers.emplace( key, sampler );
		}
		for ( const auto & [ key, geometry ] : p_builder.resources.geometries )
		{
			auto [ it, inserted ] = _resources.geometries.emplace( key, geometry );
			assert( inserted );
		}
		// Passes.
		for ( const auto & pass : p_builder.passes )
		{
			_passes.push_back( std::make_unique<Desc::Pass>( *pass ) );
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
		_resources = Desc::Resources {};
		_passes.clear();
	}

	void RenderGraph::createDefaultPipeline( const PipelineConfig & p_config, const Geometries & p_geometries )
	{
		using namespace Desc;

		_config = p_config;

		GraphBuilder g;

		// Buffers.
		g.shaderBuffer(
			"Camera",
			E_SHADER_BUFFER_KIND::PARAMETERS,
			E_BUFFER_MUTABILITY::IMMUTABLE,
			E_BUFFER_ACCESS::NONE,
			E_UPDATE_FREQUENCY::STREAM,
			15,
			{ makeUniform( "MatrixView", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixProjection", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixViewInv", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixViewInvTrans", Mat4f( MAT4F_ID ) ),
			  makeUniform( "Position", Vec3f( VEC3F_ZERO ) ),
			  makeUniform( "ClipInfos", Vec4f( VEC4F_ZERO ) ),
			  makeUniform( "Resolution", Vec2i { 0, 0 } ),
			  makeUniform( "MousePosition", Vec2i { 0, 0 } ),
			  makeUniform( "IsPerspective", uint32_t( 1 ) ) }
		);

		g.shaderBuffer(
			"ColorLayout",
			E_SHADER_BUFFER_KIND::PARAMETERS,
			E_BUFFER_MUTABILITY::IMMUTABLE,
			E_BUFFER_ACCESS::NONE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			14,
			{ makeUniformArray( "Colors", Util::Color::Rgba {}, 256 ) }
		);

		g.shaderBuffer(
			"Models",
			E_SHADER_BUFFER_KIND::STRUCTURED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			E_UPDATE_FREQUENCY::STREAM,
			13,
			{ makeUniform( "MatrixModelView", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixModelViewInv", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixNormal", Mat4f( MAT4F_ID ) ) }
		);

		g.shaderBuffer(
			"Representations",
			E_SHADER_BUFFER_KIND::STRUCTURED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			12,
			{ makeUniform( "SphereRadiusFixed", 0.0f ),
			  makeUniform( "SphereRadiusAdd", 0.0f ),
			  makeUniform( "IsSphereRadiusFixed", uint32_t( 0 ) ),
			  makeUniform( "CylinderRadius", 0.0f ),
			  makeUniform( "CylinderColorBlending", uint32_t( 0 ) ),
			  makeUniform( "RibbonColorBlending", uint32_t( 0 ) ),
			  makeUniform( "SESProbeRadius", 0.0f ),
			  makeUniform( "SESMaxProbeNeighborNb", uint32_t( 0 ) ) }
		);

		// Vertex streams and data buffers.
		g.vertexLayout(
			"Atoms",
			{
				{ "Positions", E_TYPE::VEC3F },
				{ "Colors", E_TYPE::UBYTE },
				{ "Radii", E_TYPE::FLOAT },
				{ "Ids", E_TYPE::UINT },
				{ "Flags", E_TYPE::UBYTE },
				{ "Models", E_TYPE::USHORT },
				{ "Representations", E_TYPE::UBYTE },
			}
		);

		g.pipelineBuffer( "Atoms.Positions" )
			.pipelineBuffer( "Atoms.Colors" )
			.pipelineBuffer( "Atoms.Radii" )
			.pipelineBuffer( "Atoms.Ids" )
			.pipelineBuffer( "Atoms.Flags" )
			.pipelineBuffer( "Atoms.Models" )
			.pipelineBuffer( "Atoms.Representations" )
			.pipelineBuffer( "Bonds", E_PIPELINE_BUFFER_KIND::INDEX );

		g.vertexLayout(
			"Residues",
			{
				{ "Positions", E_TYPE::VEC4F },
				{ "Directions", E_TYPE::VEC3F },
				{ "Types", E_TYPE::UBYTE },
				{ "Colors", E_TYPE::UBYTE },
				{ "Ids", E_TYPE::UINT },
				{ "Flags", E_TYPE::UBYTE },
				{ "Models", E_TYPE::USHORT },
				{ "Representations", E_TYPE::UBYTE },
			}
		);

		g.pipelineBuffer( "Residues.Positions" )
			.pipelineBuffer( "Residues.Directions" )
			.pipelineBuffer( "Residues.Types" )
			.pipelineBuffer( "Residues.Colors" )
			.pipelineBuffer( "Residues.Ids" )
			.pipelineBuffer( "Residues.Flags" )
			.pipelineBuffer( "Residues.Models" )
			.pipelineBuffer( "Residues.Representations" )
			.pipelineBuffer( "Ribbons", E_PIPELINE_BUFFER_KIND::INDEX );

		g.vertexLayout(
			"Voxels",
			{
				{ "Mins", E_TYPE::VEC3F },
				{ "Maxs", E_TYPE::VEC3F },
			}
		);

		g.pipelineBuffer( "Voxels.Mins" ).pipelineBuffer( "Voxels.Maxs" );

		// Geometries.
		g.geometry( "Spheres", "Atoms" );
		g.geometry( "Cylinders", "Atoms", "Bonds" );
		g.geometry( "Ribbons", "Residues" );
		g.geometry( "Grid", "Voxels" );

		// Textures.
		g.texture( "Geometry", E_FORMAT::RGBA32UI )
			.texture( "Color", E_FORMAT::RGBA16F )
			.texture( "Picking", E_FORMAT::RG32UI )
			.texture( "DepthRaw", E_FORMAT::DEPTH_COMPONENT32F );

		g.texture( "Depth", E_FORMAT::R32F );

		if ( p_config.enableSSAO )
		{
			constexpr size_t   noiseTextureSize = 64;
			std::vector<Vec3f> noiseData		= Util::Math::randomUniVectors( noiseTextureSize * noiseTextureSize );

			g.texture( "SSAO", E_FORMAT::R8UI );
			g.texture( "Noise", E_FORMAT::RGB16F, noiseData, Size2DAbsolute { noiseTextureSize, noiseTextureSize } );
			g.texture( "BlurX", E_FORMAT::R16F );
			g.texture( "BlurY", E_FORMAT::R16F );
		}
		else
		{
			// Used by shading pass even if SSAO disabled.
			// HALF_FLOAT value.
			std::vector<std::byte> emptyData = { std::byte { 0x00 }, std::byte { 0x3C } };
			g.texture( "BlurY", E_FORMAT::R16F, emptyData, Size2DAbsolute { 1, 1 } );
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
		g.defaultSampler();

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
			.settings( { E_SETTING::CLEAR_COLOR, E_SETTING::CLEAR_DEPTH, E_SETTING::ENABLE_DEPTH } )
			.in( E_RESOURCE_TYPE::GEOMETRY, "Spheres" )
			.in( E_RESOURCE_TYPE::GEOMETRY, "Cylinders" )
			.in( E_RESOURCE_TYPE::GEOMETRY, "Ribbons" )
			.in( E_RESOURCE_TYPE::GEOMETRY, "Grid" )
			.out( "Geometry" )
			.out( "Color" )
			.out( "Picking" )
			.out( "DepthRaw" )
			.program( "Sphere" )
			.shadersDir( "sphere" )
			.draw( "Spheres", E_PRIMITIVE::POINTS, &p_geometries.spheres.drawRanges )
			.endProgram()
			.program( "Cylinder" )
			.shadersDir( "cylinder" )
			.draw( "Cylinders", E_PRIMITIVE::LINES, nullptr, &p_geometries.cylinders.drawRanges )
			.endProgram()
			.program( "Ribbon" )
			.shadersDir( "ribbon" )
			.draw( "Ribbons", E_PRIMITIVE::PATCHES, nullptr, &p_geometries.ribbons.drawRanges )
			.endProgram()
			.program( "Voxel" )
			.shadersDir( "voxel" )
			.draw( "Grid", E_PRIMITIVE::POINTS, &p_geometries.voxels.drawRanges )
			.endProgram()
			.endPass();

		// Linearize depth.
		g.pass( "LinearizeDepth" )
			.in( "DepthRaw" )
			.out( "Depth" )
			.program( "LinearizeDepth" )
			.shaders( { "default.vert", "linearize_depth.frag" } )
			.endProgram()
			.endPass();

		if ( p_config.enableSSAO )
		{
			// SSAO.
			g.pass( "SSAO" )
				.in( "Geometry" )
				.in( "Noise", "NearestRepeat" )
				.in( "Depth" )
				.out( "SSAO" )
				.program( "SSAO" )
				.shaders( { "default.vert", "ssao.frag" } )
				.uniform( "Intensity", SSAO_INTENSITY_DEFAULT, std::pair { SSAO_INTENSITY_MIN, SSAO_INTENSITY_MAX } )
				.endProgram()
				.endPass();

			// BlurX.
			g.pass( "BlurX" )
				.in( "SSAO" )
				.in( "Depth" )
				.out( "BlurX" )
				.program( "BlurX" )
				.shaders( { "default.vert", "blur.frag" } )
				.uniform( "Direction", Vec2i( 1, 0 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.endProgram()
				.endPass();
			// BlurY.
			g.pass( "BlurY" )
				.in( "BlurX" )
				.in( "Depth" )
				.out( "BlurY" )
				.program( "BlurY" )
				.shaders( { "default.vert", "blur.frag" } )
				.uniform( "Direction", Vec2i( 0, 1 ) )
				.uniform( "Size", BLUR_SIZE_DEFAULT, std::pair { BLUR_SIZE_MIN, BLUR_SIZE_MAX } )
				.endProgram()
				.endPass();
		}

		// Shading.
		g.pass( "Shading" )
			.in( "Geometry" )
			.in( "Color" )
			.in( "BlurY", p_config.enableSSAO ? "Default" : "NearestRepeat" ) // Repeat empty texture 1x1 if not SSAO.
			.out( "Shaded" )
			.program( "Shading" )
			.shaders( { "default.vert", "shading.frag" } )
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
				.shaders( { "default.vert", "outline.frag" } )
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
				.in( p_config.enableOutline ? "Outline" : "Shaded" )
				.in( "Depth" )
				.out( "Selection" )
				.program( "Selection" )
				.shaders( { "default.vert", "selection.frag" } )
				.uniform( "Color", COLOR_SELECTION_DEFAULT )
				.endProgram()
				.endPass();
		}

		// FXAA.
		g.pass( "FXAA" )
			.in( p_config.enableSelection ? "Selection"
				 : p_config.enableOutline ? "Outline"
										  : "Shaded" )
			.out( "FXAA" )
			.program( "FXAA" )
			.shaders( { "default.vert", "fxaa.frag" } )
			.endProgram()
			.endPass();

		// Pixelize
		/*
		g.pass( "Pixelize" )
			.in( "FXAA" )
			.out( "Pixelize" )
			.program( "Pixelize" )
			.shaders( {  "default.vert" ,  "pixelize.frag"  } )
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
			.shaders( {  "default.vert" ,  "crt.frag"  } )
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
			.shaders( {  "default.vert" ,  "chromatic_aberration.frag"  } )
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
			.shaders( {  "default.vert" ,  "colorize.frag"  } )
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
			.shaders( {  "default.vert" ,  "debug.frag"  } )
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
