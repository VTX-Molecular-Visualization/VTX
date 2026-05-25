#include "renderer/builder/render_graph_build.hpp"
#include "renderer/graphics_config.hpp"
#include "renderer/representation.hpp"
#include <util/chrono.hpp>

namespace VTX::Renderer::Builder
{
	GraphBuilder DefaultRenderGraph::build(
		const PipelineConfig & p_config,
		const Layouts &		   p_layouts,
		const Geometries &	   p_geometries
	)
	{
		using namespace Desc;
		using CylinderGeometry = VTX::Renderer::Geometry::Cylinder;
		using GridGeometry	   = VTX::Renderer::Geometry::Grid;
		using RibbonGeometry   = VTX::Renderer::Geometry::Ribbon;
		using SESGeometry	   = VTX::Renderer::Geometry::SES;
		using SphereGeometry   = VTX::Renderer::Geometry::Sphere;

		GraphBuilder g;

		// Buffers.
		g.buffer(
			"Camera",
			E_BUFFER_USAGE::UNIFORM,
			E_UPDATE_FREQUENCY::STREAM,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::IMMUTABLE,
			E_BUFFER_ACCESS::NONE,
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

		g.buffer(
			"ColorLayout",
			E_BUFFER_USAGE::UNIFORM,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::IMMUTABLE,
			E_BUFFER_ACCESS::NONE,
			14,
			{ makeUniformArray( "Colors", Util::Color::Rgba {}, 256 ) }
		);

		g.buffer(
			"Models",
			E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::STREAM,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			13,
			{ makeUniform( "MatrixModelView", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixModelViewInv", Mat4f( MAT4F_ID ) ),
			  makeUniform( "MatrixNormal", Mat4f( MAT4F_ID ) ) }
		);

		g.buffer(
			"Representations",
			E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			12,
			{ makeUniform( "SphereRadiusFixed", 0.0f ),
			  makeUniform( "SphereRadiusAdd", 0.0f ),
			  makeUniform( "IsSphereRadiusFixed", uint32_t( 0 ) ),
			  makeUniform( "CylinderRadius", 0.0f ),
			  makeUniform( "CylinderColorBlending", uint32_t( 0 ) ),
			  makeUniform( "RibbonColorBlending", uint32_t( 0 ) ),
			  makeUniform( "SESProbeRadius", 0.0f ),
			  makeUniform( "SESMaxProbeNeighborNb", SESGeometry::MAX_PROBE_NEIGHBOR_NB ) }
		);

		// Vertex streams.
		g.vertexLayout( SphereGeometry::VERTEX_LAYOUT_ATOMS, p_layouts.atoms );
		g.buffer( Layout::Atoms::ATOMS_POSITIONS, E_BUFFER_USAGE::CUDA_READ, E_UPDATE_FREQUENCY::DYNAMIC );
		g.buffer( Layout::Atoms::ATOMS_SYMBOLS, E_BUFFER_USAGE::CUDA_READ, E_UPDATE_FREQUENCY::DYNAMIC );
		g.buffer(
			Layout::Atoms::ATOMS_COLORS,
			E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			Layout::Atoms::BINDING_ATOMS_COLORS
		);
		g.buffer(
			Layout::Atoms::ATOMS_FLAGS,
			E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			Layout::Atoms::BINDING_ATOMS_FLAGS
		);
		g.vertexLayout( RibbonGeometry::VERTEX_LAYOUT_RESIDUES, p_layouts.residues );
		g.vertexLayout( GridGeometry::VERTEX_LAYOUT_VOXELS, p_layouts.voxels );
		g.buffer(
			SphereGeometry::INDIRECT_SPHERES,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			SphereGeometry::BINDING_INDIRECT_SPHERES
		);
		g.buffer(
			CylinderGeometry::INDIRECT_CYLINDERS,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			CylinderGeometry::BINDING_INDIRECT_CYLINDERS
		);
		g.buffer(
			RibbonGeometry::INDIRECT_RIBBONS,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			RibbonGeometry::BINDING_INDIRECT_RIBBONS
		);
		g.buffer(
			GridGeometry::INDIRECT_GRID,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			GridGeometry::BINDING_INDIRECT_GRID
		);
		g.buffer(
			SESGeometry::BUFFER_ATOMS,
			E_BUFFER_USAGE::STORAGE | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			1
		);
		g.buffer(
			SESGeometry::BUFFER_ATOM_IDS,
			E_BUFFER_USAGE::STORAGE | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			7
		);
		g.buffer(
			SESGeometry::BUFFER_PROBES,
			E_BUFFER_USAGE::STORAGE | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			3
		);
		g.buffer(
			SESGeometry::BUFFER_PROBE_ATOM_INDICES,
			E_BUFFER_USAGE::STORAGE | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			4
		);
		g.buffer(
			SESGeometry::BUFFER_PROBE_NEIGHBORS,
			E_BUFFER_USAGE::STORAGE | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			5
		);
		g.buffer(
			SESGeometry::BUFFER_SECTORS,
			E_BUFFER_USAGE::STORAGE | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			6
		);
		g.buffer(
			SESGeometry::BUFFER_CONVEX_PATCH_ELEMENTS,
			E_BUFFER_USAGE::VERTEX | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			std::nullopt
		);
		g.buffer(
			SESGeometry::BUFFER_CIRCLE_PATCH_ATOMS,
			E_BUFFER_USAGE::VERTEX | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			std::nullopt
		);
		g.buffer(
			SESGeometry::BUFFER_SEGMENT_PATCH_IDS,
			E_BUFFER_USAGE::VERTEX | E_BUFFER_USAGE::STORAGE | E_BUFFER_USAGE::CUDA_WRITE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			2
		);
		g.buffer(
			SESGeometry::INDEX_CONVEX_PATCHES,
			E_BUFFER_USAGE::INDEX,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			std::nullopt
		);
		g.buffer(
			SESGeometry::INDEX_CIRCLE_PATCHES,
			E_BUFFER_USAGE::INDEX,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			std::nullopt
		);
		g.buffer(
			SESGeometry::INDEX_SEGMENT_PATCHES,
			E_BUFFER_USAGE::INDEX,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			std::nullopt
		);
		g.buffer(
			SESGeometry::INDEX_CONCAVE_PATCHES,
			E_BUFFER_USAGE::INDEX,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			std::nullopt
		);
		g.buffer(
			SESGeometry::INDIRECT_CONVEX_PATCHES,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			SESGeometry::BINDING_INDIRECT_CONVEX_PATCHES
		);
		g.buffer(
			SESGeometry::INDIRECT_CIRCLE_PATCHES,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			SESGeometry::BINDING_INDIRECT_CIRCLE_PATCHES
		);
		g.buffer(
			SESGeometry::INDIRECT_SEGMENT_PATCHES,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			SESGeometry::BINDING_INDIRECT_SEGMENT_PATCHES
		);
		g.buffer(
			SESGeometry::INDIRECT_CONCAVE_PATCHES,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::CHUNKED,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			SESGeometry::BINDING_INDIRECT_CONCAVE_PATCHES
		);
		g.vertexLayout(
			SESGeometry::GEOMETRY_CONVEX_PATCHES, { { SESGeometry::BUFFER_CONVEX_PATCH_ELEMENTS, E_TYPE::VEC2U } }
		);
		g.vertexLayout(
			SESGeometry::GEOMETRY_CIRCLE_PATCHES, { { SESGeometry::BUFFER_CIRCLE_PATCH_ATOMS, E_TYPE::VEC2U } }
		);
		g.vertexLayout(
			SESGeometry::GEOMETRY_SEGMENT_PATCHES, { { SESGeometry::BUFFER_SEGMENT_PATCH_IDS, E_TYPE::VEC4U } }
		);
		g.vertexLayout( SESGeometry::GEOMETRY_CONCAVE_PATCHES, VertexLayout {} );

		// Geometries.
		g.geometry( SphereGeometry::GEOMETRY_SPHERES, p_geometries.spheres );
		g.geometry( CylinderGeometry::GEOMETRY_CYLINDERS, p_geometries.cylinders );
		g.geometry( RibbonGeometry::GEOMETRY_RIBBONS, p_geometries.ribbons );
		g.geometry( GridGeometry::GEOMETRY_GRID, p_geometries.grid );
		g.geometry( SESGeometry::GEOMETRY_CONVEX_PATCHES, p_geometries.ses.convexPatches );
		g.geometry( SESGeometry::GEOMETRY_CIRCLE_PATCHES, p_geometries.ses.circlePatches );
		g.geometry( SESGeometry::GEOMETRY_SEGMENT_PATCHES, p_geometries.ses.segmentPatches );
		g.geometry( SESGeometry::GEOMETRY_CONCAVE_PATCHES, p_geometries.ses.concavePatches );

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
		g.externalPass( SESGeometry::PASS_COMPUTE ).endPass();

		// Geometric.
		g.pass( "Geometric" )
			.settings( { E_SETTING::CLEAR_COLOR, E_SETTING::CLEAR_DEPTH, E_SETTING::ENABLE_DEPTH } )
			.in( E_RESOURCE_TYPE::GEOMETRY, SphereGeometry::GEOMETRY_SPHERES )
			.in( E_RESOURCE_TYPE::GEOMETRY, CylinderGeometry::GEOMETRY_CYLINDERS )
			.in( E_RESOURCE_TYPE::GEOMETRY, RibbonGeometry::GEOMETRY_RIBBONS )
			.in( E_RESOURCE_TYPE::GEOMETRY, GridGeometry::GEOMETRY_GRID )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_CONVEX_PATCHES )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_CIRCLE_PATCHES )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_SEGMENT_PATCHES )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_CONCAVE_PATCHES )
			.in( E_RESOURCE_TYPE::BUFFER, SphereGeometry::INDIRECT_SPHERES )
			.in( E_RESOURCE_TYPE::BUFFER, CylinderGeometry::INDIRECT_CYLINDERS )
			.in( E_RESOURCE_TYPE::BUFFER, RibbonGeometry::INDIRECT_RIBBONS )
			.in( E_RESOURCE_TYPE::BUFFER, GridGeometry::INDIRECT_GRID )
			.in( E_RESOURCE_TYPE::BUFFER, SESGeometry::INDIRECT_CONVEX_PATCHES )
			.in( E_RESOURCE_TYPE::BUFFER, SESGeometry::INDIRECT_CIRCLE_PATCHES )
			.in( E_RESOURCE_TYPE::BUFFER, SESGeometry::INDIRECT_SEGMENT_PATCHES )
			.in( E_RESOURCE_TYPE::BUFFER, SESGeometry::INDIRECT_CONCAVE_PATCHES )
			.out( "Geometry" )
			.out( "Color" )
			.out( "Picking" )
			.out( "DepthRaw" )
			.program( "Sphere" )
			.shadersDir( "sphere" )
			.draw( SphereGeometry::GEOMETRY_SPHERES, E_PRIMITIVE::POINTS )
			.endProgram()
			.program( "Cylinder" )
			.shadersDir( "cylinder" )
			.draw( CylinderGeometry::GEOMETRY_CYLINDERS, E_PRIMITIVE::LINES )
			.endProgram()
			.program( "Ribbon" )
			.shadersDir( "ribbon" )
			.draw( RibbonGeometry::GEOMETRY_RIBBONS, E_PRIMITIVE::PATCHES )
			.endProgram()
			.program( "Voxel" )
			.shadersDir( "voxel" )
			.draw( GridGeometry::GEOMETRY_GRID, E_PRIMITIVE::POINTS )
			.endProgram()
			.program( "SES.ConvexPatch" )
			.shadersDir( "ses/sesdf/convex" )
			.draw( SESGeometry::GEOMETRY_CONVEX_PATCHES, E_PRIMITIVE::POINTS )
			.endProgram()
			.program( "SES.CirclePatch" )
			.shadersDir( "ses/sesdf/circle" )
			.draw( SESGeometry::GEOMETRY_CIRCLE_PATCHES, E_PRIMITIVE::POINTS )
			.endProgram()
			.program( "SES.SegmentPatch" )
			.shadersDir( "ses/sesdf/segment" )
			.draw( SESGeometry::GEOMETRY_SEGMENT_PATCHES, E_PRIMITIVE::POINTS )
			.endProgram()
			.program( "SES.ConcavePatch" )
			.shadersDir( "ses/sesdf/concave" )
			.draw( SESGeometry::GEOMETRY_CONCAVE_PATCHES, E_PRIMITIVE::POINTS )
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

		return g;
	}

	bool RenderGraphRuntime::refreshGraph(
		const GraphicsConfig &			 p_config,
		std::optional<PipelineConfig> & p_currentConfig,
		RenderGraph &					 p_graph,
		Desc::RenderQueue &			 p_queue,
		const Layouts &				 p_layouts,
		const Geometries &			 p_geometries
	)
	{
		Util::ScopedChrono timer( "[RENDERER] _refreshGraph" );

		PipelineConfig config;
		config.enableSSAO	   = p_config.activeSSAO;
		config.enableOutline   = p_config.activeOutline;
		config.enableSelection = p_config.activeSelection;

		if ( p_currentConfig && *p_currentConfig == config )
		{
			return false;
		}

		p_currentConfig = config;
		p_graph.set( DefaultRenderGraph::build( config, p_layouts, p_geometries ) );
		p_queue = p_graph.build();

		return true;
	}

	bool RenderGraphRuntime::syncGeometryChunks( RenderGraph & p_graph, const Geometries & p_geometries )
	{
		bool changed = false;

		auto sync = [ & ]( const Desc::Key & p_geometry, const Desc::Geometry & p_source )
		{
			if ( not p_graph.getResources().geometries.contains( p_geometry ) )
			{
				return;
			}

			changed = p_graph.setGeometryChunks( p_geometry, p_source.chunks ) || changed;
		};

		sync( Geometry::SES::GEOMETRY_CONVEX_PATCHES, p_geometries.ses.convexPatches );
		sync( Geometry::SES::GEOMETRY_CIRCLE_PATCHES, p_geometries.ses.circlePatches );
		sync( Geometry::SES::GEOMETRY_SEGMENT_PATCHES, p_geometries.ses.segmentPatches );
		sync( Geometry::SES::GEOMETRY_CONCAVE_PATCHES, p_geometries.ses.concavePatches );

		return changed;
	}

	void RenderGraphRuntime::bindExternalPasses(
		Context::ContextWrapper & p_context,
		const uintptr_t			 p_function,
		const uintptr_t			 p_contextPtr
	)
	{
		if ( not p_context.containsPass( Geometry::SES::PASS_COMPUTE ) )
		{
			return;
		}

		p_context.setExternalPass( Geometry::SES::PASS_COMPUTE, p_function, p_contextPtr );
	}

	void RenderGraphRuntime::markSESDirty( Context::ContextWrapper & p_context, const Geometries & p_geometries )
	{
		if ( p_context.containsPass( Geometry::SES::PASS_COMPUTE ) && p_geometries.ses.hasPendingCompute() )
		{
			p_context.markPassDirty( Geometry::SES::PASS_COMPUTE );
		}
	}

	void RenderGraphRuntime::rebuildCommandBuffer(
		Context::ContextWrapper & p_context,
		const Desc::RenderQueue & p_queue,
		const Desc::Resources &	 p_resources,
		const uintptr_t			 p_externalFunction,
		const uintptr_t			 p_externalContext
	)
	{
		p_context.build( p_queue, p_resources );
		bindExternalPasses( p_context, p_externalFunction, p_externalContext );
	}
} // namespace VTX::Renderer::Builder
