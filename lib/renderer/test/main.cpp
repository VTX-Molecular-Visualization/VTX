#include <catch2/catch_test_macros.hpp>
#include <renderer/graph_builder.hpp>
#include <renderer/render_graph.hpp>
#include <util/exceptions.hpp>
#include <util/math.hpp>

using namespace VTX::Renderer;

namespace
{
	using namespace VTX;

	GraphBuilder makeLinearGraphABC()
	{
		GraphBuilder g;

		g.texture( "A_out", E_FORMAT::RGBA16F );
		g.texture( "B_out", E_FORMAT::RGBA16F );
		g.texture( "C_out", E_FORMAT::RGBA16F );

		g.pass( "A" )
			.out( "A_out" )
			.program( "ProgA" )
			.shaders( { FilePath( "a.vert" ), FilePath( "a.frag" ) } )
			.endProgram()
			.endPass();

		g.pass( "B" )
			.in( "A_out" )
			.out( "B_out" )
			.program( "ProgB" )
			.shaders( { FilePath( "b.vert" ), FilePath( "b.frag" ) } )
			.endProgram()
			.endPass();

		g.pass( "C" )
			.in( "B_out" )
			.out( "C_out" )
			.program( "ProgC" )
			.shaders( { FilePath( "c.vert" ), FilePath( "c.frag" ) } )
			.endProgram()
			.endPass();

		return g;
	}
} // namespace

TEST_CASE( "RenderGraph: simple linear graph builds and preserves order", "[renderer][graph]" )
{
	RenderGraph graph;
	auto		builder = makeLinearGraphABC();

	graph.set( std::move( builder ) );

	RenderQueue queue;
	REQUIRE_NOTHROW( queue = graph.build() );

	REQUIRE( queue.size() == 3 );
	CHECK( queue[ 0 ]->name == "A" );
	CHECK( queue[ 1 ]->name == "B" );
	CHECK( queue[ 2 ]->name == "C" );
}

TEST_CASE( "RenderGraph: missing input causes build() to throw", "[renderer][graph]" )
{
	GraphBuilder g;

	g.texture( "B_out", E_FORMAT::RGBA16F );

	g.pass( "B" )
		.in( "A_out" ) // Missing.
		.out( "B_out" )
		.program( "ProgB" )
		.shaders( { FilePath( "b.vert" ), FilePath( "b.frag" ) } )
		.endProgram()
		.endPass();

	RenderGraph graph;
	graph.set( std::move( g ) );

	REQUIRE_THROWS_AS( graph.build(), GraphicException );
}

TEST_CASE( "RenderGraph: external texture with data is accepted", "[renderer][graph]" )
{
	GraphBuilder g;

	constexpr std::size_t noiseCount = 16;
	std::vector<Vec3f>	  noiseData( noiseCount );
	std::generate( noiseData.begin(), noiseData.end(), [] { return Vec3f( 0.5f, 0.25f, 0.0f ); } );
	g.texture( "Noise", E_FORMAT::RGB16F, noiseData );

	g.texture( "SSAO", E_FORMAT::R8 );

	g.pass( "SSAO" )
		.in( "Noise" )
		.out( "SSAO" )
		.program( "ProgSSAO" )
		.shaders( { FilePath( "ssao.vert" ), FilePath( "ssao.frag" ) } )
		.endProgram()
		.endPass();

	RenderGraph graph;
	graph.set( std::move( g ) );

	RenderQueue queue;
	REQUIRE_NOTHROW( queue = graph.build() );

	REQUIRE( queue.size() == 1 );
	CHECK( queue[ 0 ]->name == "SSAO" );
}

TEST_CASE( "RenderGraph: add() merges builders and build() still works", "[renderer][graph]" )
{
	GraphBuilder g1;
	g1.texture( "A_out", E_FORMAT::RGBA16F );

	g1.pass( "A" )
		.out( "A_out" )
		.program( "ProgA" )
		.shaders( { FilePath( "a.vert" ), FilePath( "a.frag" ) } )
		.endProgram()
		.endPass();

	GraphBuilder g2;

	g2.texture( "B_out", E_FORMAT::RGBA16F );

	g2.pass( "B" )
		.in( "A_out" )
		.out( "B_out" )
		.program( "ProgB" )
		.shaders( { FilePath( "b.vert" ), FilePath( "b.frag" ) } )
		.endProgram()
		.endPass();

	RenderGraph graph;

	graph.set( std::move( g1 ) );

	graph.add( g2 );

	RenderQueue queue;
	REQUIRE_NOTHROW( queue = graph.build() );

	REQUIRE( queue.size() == 2 );
	CHECK( queue[ 0 ]->name == "A" );
	CHECK( queue[ 1 ]->name == "B" );
}

TEST_CASE( "RenderGraph: default pipeline builds with all features enabled", "[renderer][graph]" )
{
	RenderGraph::PipelineConfig cfg;
	cfg.enableSSAO		= true;
	cfg.enableOutline	= true;
	cfg.enableSelection = true;

	RenderGraph graph;
	graph.createDefaultPipeline( cfg );

	RenderQueue queue;
	REQUIRE_NOTHROW( queue = graph.build() );

	REQUIRE_FALSE( queue.empty() );

	bool hasGeometric = false;
	bool hasShading	  = false;
	bool hasFXAA	  = false;

	for ( const Pass * p : queue )
	{
		if ( p->name == "Geometric" )
			hasGeometric = true;
		if ( p->name == "Shading" )
			hasShading = true;
		if ( p->name == "FXAA" )
			hasFXAA = true;
	}

	CHECK( hasGeometric );
	CHECK( hasShading );
	CHECK( hasFXAA );
}
