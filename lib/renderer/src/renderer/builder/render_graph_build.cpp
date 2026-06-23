#include "renderer/builder/render_graph_build.hpp"
#include "renderer/builder/post_process/blur.hpp"
#include "renderer/builder/post_process/chromatic_aberration.hpp"
#include "renderer/builder/post_process/crt.hpp"
#include "renderer/builder/post_process/fxaa.hpp"
#include "renderer/builder/post_process/linearize_depth.hpp"
#include "renderer/builder/post_process/outline.hpp"
#include "renderer/builder/post_process/pixelize.hpp"
#include "renderer/builder/post_process/selection.hpp"
#include "renderer/builder/post_process/shading.hpp"
#include "renderer/builder/post_process/ssao.hpp"
#include "renderer/builder/post_process/tone_mapping.hpp"
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
		// Util::ScopedChrono timer( "[BUILDER] DefaultRenderGraph::build" );

		using namespace Desc;
		using CylinderGeometry = VTX::Renderer::Geometry::Cylinder;
		using GridGeometry	   = VTX::Renderer::Geometry::Grid;
		using RibbonGeometry   = VTX::Renderer::Geometry::Ribbon;
		using SESGeometry	   = VTX::Renderer::Geometry::SES;
		using SphereGeometry   = VTX::Renderer::Geometry::Sphere;
		using TriangleGeometry = VTX::Renderer::Geometry::Triangle;

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
			  makeUniform( "IsPerspective", uint32_t( 1 ) ),
			  makeUniform( "TanHalfFov", 1.f ) }
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
		g.vertexLayout( TriangleGeometry::VERTEX_LAYOUT_MESHES, p_layouts.meshes );
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
			TriangleGeometry::INDIRECT_TRIANGLES,
			E_BUFFER_USAGE::INDIRECT | E_BUFFER_USAGE::STORAGE,
			E_UPDATE_FREQUENCY::DYNAMIC,
			E_BUFFER_ALLOCATION::SINGLE,
			E_BUFFER_MUTABILITY::MUTABLE,
			E_BUFFER_ACCESS::NONE,
			TriangleGeometry::BINDING_INDIRECT_TRIANGLES
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
		g.geometry( TriangleGeometry::GEOMETRY_TRIANGLES, p_geometries.triangles );
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
			const float		   ssaoSize			= 1.f / p_config.ssaoScale;

			g.texture( "SSAO", E_FORMAT::R8UI, Size2DRelative { ssaoSize, ssaoSize } );
			g.texture( "Noise", E_FORMAT::RGB16F, noiseData, Size2DAbsolute { noiseTextureSize, noiseTextureSize } );
			g.texture( "BlurX", E_FORMAT::R16F, Size2DRelative { ssaoSize, ssaoSize } );
			g.texture( "BlurY", E_FORMAT::R16F, Size2DRelative { ssaoSize, ssaoSize } );
		}
		else
		{
			// Used by shading pass even if SSAO disabled.
			// HALF_FLOAT value.
			std::vector<std::byte> emptyData = { std::byte { 0x00 }, std::byte { 0x3C } };
			g.texture( "BlurY", E_FORMAT::R16F, emptyData, Size2DAbsolute { 1, 1 } );
		}

		g.texture( "Shaded", E_FORMAT::RGBA16F );

		const uint32_t environmentFaceSize = p_config.enableEnvironment ? p_config.environmentFaceSize : 1;
		g.texture(
			PostProcess::Shading::ENVIRONMENT_TEXTURE,
			E_FORMAT::RGBA16F,
			Size2DAbsolute { environmentFaceSize, environmentFaceSize },
			E_TEXTURE_TARGET::CUBEMAP,
			true
		);

		if ( p_config.shadingMode == E_SHADING::PBR )
		{
			g.texture( PostProcess::ToneMapping::PASS, E_FORMAT::RGBA16F );
		}

		if ( p_config.enableOutline )
		{
			g.texture( "Outline", E_FORMAT::RGBA16F );
		}

		if ( p_config.enableSelection )
		{
			g.texture( "Selection", E_FORMAT::RGBA16F );
		}

		g.texture( "FXAA", E_FORMAT::SRGB8_ALPHA8 );

		if ( p_config.enableChromaticAberration )
		{
			g.texture( "ChromaticAberration", E_FORMAT::RGBA16F );
		}

		if ( p_config.enablePixelize )
		{
			g.texture( "Pixelize", E_FORMAT::RGBA16F );
		}

		if ( p_config.enableCRT )
		{
			g.texture( "CRT", E_FORMAT::RGBA16F );
		}

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

		g.sampler(
			PostProcess::Shading::ENVIRONMENT_SAMPLER,
			E_WRAPPING::CLAMP_TO_EDGE,
			E_WRAPPING::CLAMP_TO_EDGE,
			E_FILTERING::LINEAR_MIPMAP_LINEAR,
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
			.in( E_RESOURCE_TYPE::GEOMETRY, TriangleGeometry::GEOMETRY_TRIANGLES )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_CONVEX_PATCHES )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_CIRCLE_PATCHES )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_SEGMENT_PATCHES )
			.in( E_RESOURCE_TYPE::GEOMETRY, SESGeometry::GEOMETRY_CONCAVE_PATCHES )
			.in( E_RESOURCE_TYPE::BUFFER, SphereGeometry::INDIRECT_SPHERES )
			.in( E_RESOURCE_TYPE::BUFFER, CylinderGeometry::INDIRECT_CYLINDERS )
			.in( E_RESOURCE_TYPE::BUFFER, RibbonGeometry::INDIRECT_RIBBONS )
			.in( E_RESOURCE_TYPE::BUFFER, GridGeometry::INDIRECT_GRID )
			.in( E_RESOURCE_TYPE::BUFFER, TriangleGeometry::INDIRECT_TRIANGLES )
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
			.program( "Triangle" )
			.shadersDir( "triangle" )
			.draw( TriangleGeometry::GEOMETRY_TRIANGLES, E_PRIMITIVE::TRIANGLES )
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

		PostProcess::LinearizeDepth::build( g );

		if ( p_config.enableSSAO )
		{
			PostProcess::SSAO::build( g, p_config.ssaoMethod );
			PostProcess::BlurX::build( g, PostProcess::SSAO::PASS );
			PostProcess::BlurY::build( g, PostProcess::BlurX::PASS );
		}

		PostProcess::Shading::build( g, p_config.enableSSAO, p_config.shadingMode );

		const Desc::Key postChromatic = p_config.enableChromaticAberration ? PostProcess::ChromaticAberration::PASS
																		   : PostProcess::Shading::OUTPUT;

		if ( p_config.enableChromaticAberration )
		{
			PostProcess::ChromaticAberration::build( g, PostProcess::Shading::OUTPUT );
		}

		const Desc::Key postEffects = p_config.enablePixelize ? PostProcess::Pixelize::PASS : postChromatic;

		if ( p_config.enablePixelize )
		{
			PostProcess::Pixelize::build( g, postChromatic );
		}

		if ( p_config.enableOutline )
		{
			PostProcess::Outline::build( g, postEffects );
		}

		if ( p_config.enableSelection )
		{
			PostProcess::Selection::build( g, p_config.enableOutline ? PostProcess::Outline::PASS : postEffects );
		}

		const Desc::Key postSelection = p_config.enableSelection ? PostProcess::Selection::PASS
										: p_config.enableOutline ? PostProcess::Outline::PASS
																 : postEffects;

		if ( p_config.enableCRT )
		{
			PostProcess::CRT::build( g, postSelection );
		}

		const Desc::Key postCRT = p_config.enableCRT ? PostProcess::CRT::PASS : postSelection;
		if ( p_config.shadingMode == E_SHADING::PBR )
		{
			PostProcess::ToneMapping::build( g, postCRT );
		}
		PostProcess::FXAA::build(
			g, p_config.shadingMode == E_SHADING::PBR ? PostProcess::ToneMapping::PASS : postCRT
		);

		// Debug
		/*
		g.pass( "Debug" )
			.in( "FXAA" )
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

	PipelineConfig RenderGraphRuntime::pipelineConfig( const GraphicsConfig & p_config )
	{
		PipelineConfig config;
		config.enableSSAO				 = p_config.ssao.has_value();
		config.ssaoMethod				 = p_config.ssao ? p_config.ssao->method : SSAO_METHOD_DEFAULT;
		config.ssaoScale				 = p_config.ssao ? p_config.ssao->scale : SSAO_SCALE_DEFAULT;
		config.shadingMode				 = p_config.shading.mode;
		config.enableOutline			 = p_config.outline.has_value();
		config.enableSelection			 = p_config.selection.has_value();
		config.enableChromaticAberration = p_config.chromaticAberration.has_value();
		config.enablePixelize			 = p_config.pixelize.has_value();
		config.enableCRT				 = p_config.crt.has_value();
		config.enableEnvironment		 = p_config.shading.environmentPath.has_value();
		config.environmentFaceSize		 = config.enableEnvironment ? p_config.shading.environmentFaceSize : 1;

		return config;
	}

	bool RenderGraphRuntime::refreshGraph(
		const GraphicsConfig &			p_config,
		std::optional<PipelineConfig> & p_currentConfig,
		RenderGraph &					p_graph,
		Desc::RenderQueue &				p_queue,
		const Layouts &					p_layouts,
		const Geometries &				p_geometries
	)
	{
		// Util::ScopedChrono timer( "[BUILDER] RenderGraphRuntime::refreshGraph" );

		const PipelineConfig config = pipelineConfig( p_config );

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
		// Util::ScopedChrono timer( "[BUILDER] RenderGraphRuntime::syncGeometryChunks" );

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
		const uintptr_t			  p_function,
		const uintptr_t			  p_contextPtr
	)
	{
		// Util::ScopedChrono timer( "[BUILDER] RenderGraphRuntime::bindExternalPasses" );

		if ( not p_context.containsPass( Geometry::SES::PASS_COMPUTE ) )
		{
			return;
		}

		p_context.setExternalPass( Geometry::SES::PASS_COMPUTE, p_function, p_contextPtr );
	}

	void RenderGraphRuntime::markSESDirty( Context::ContextWrapper & p_context, const Geometries & p_geometries )
	{
		// Util::ScopedChrono timer( "[BUILDER] RenderGraphRuntime::markSESDirty" );

		if ( p_context.containsPass( Geometry::SES::PASS_COMPUTE ) && p_geometries.ses.hasPendingCompute() )
		{
			p_context.markPassDirty( Geometry::SES::PASS_COMPUTE );
		}
	}

	void RenderGraphRuntime::rebuildCommandBuffer(
		Context::ContextWrapper & p_context,
		const Desc::RenderQueue & p_queue,
		const Desc::Resources &	  p_resources,
		const uintptr_t			  p_externalFunction,
		const uintptr_t			  p_externalContext
	)
	{
		// Util::ScopedChrono timer( "[BUILDER] RenderGraphRuntime::rebuildCommandBuffer" );

		p_context.build( p_queue, p_resources );
		bindExternalPasses( p_context, p_externalFunction, p_externalContext );
	}
} // namespace VTX::Renderer::Builder
