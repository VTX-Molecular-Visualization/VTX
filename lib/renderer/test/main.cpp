#include <catch2/catch_test_macros.hpp>
#include <renderer/context/command_buffer.hpp>
#include <renderer/graph_builder.hpp>
#include <renderer/render_graph.hpp>
#include <util/exceptions.hpp>
#include <util/math.hpp>

using namespace VTX::Renderer;
using namespace VTX::Renderer::Context;

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

static bool bytesEqual( const void * p_a, const void * p_b, size_t p_n ) { return std::memcmp( p_a, p_b, p_n ) == 0; }

TEST_CASE( "CommandBuffer: push command without payload sets NO_PAYLOAD" )
{
	CommandBuffer cb;

	cb.push<E_COMMAND::CLEAR>();

	REQUIRE( cb.commands.size() == 1 );
	REQUIRE( cb.commands[ 0 ].type == E_COMMAND::CLEAR );
	REQUIRE( cb.commands[ 0 ].payloadOffset == NO_PAYLOAD );
	REQUIRE( cb.payload.empty() );
}

TEST_CASE( "CommandBuffer: push command with payload stores bytes and is readable" )
{
	CommandBuffer cb;

	PayloadViewport vp {};
	vp.x = 10;
	vp.y = 20;
	vp.w = 640;
	vp.h = 480;

	cb.push<E_COMMAND::SET_VIEWPORT>( vp );

	REQUIRE( cb.commands.size() == 1 );
	REQUIRE( cb.commands[ 0 ].type == E_COMMAND::SET_VIEWPORT );

	const uint32_t off = cb.commands[ 0 ].payloadOffset;
	REQUIRE( off != NO_PAYLOAD );
	REQUIRE( ( off % alignof( PayloadViewport ) ) == 0 );
	REQUIRE( off + sizeof( PayloadViewport ) <= cb.payload.size() );

	const auto & back = cb.getPayload<PayloadViewport>( off );
	REQUIRE( bytesEqual( &back, &vp, sizeof( PayloadViewport ) ) );
}

struct alignas( 16 ) A16
{
	std::uint32_t a, b, c, d;
};

TEST_CASE( "CommandBuffer: pushPayload aligns and pads correctly" )
{
	CommandBuffer cb;

	PayloadDraw d {};
	d.vertexCount		= 1;
	d.instanceCount		= 2;
	d.firstVertex		= 3;
	d.firstInstance		= 4;
	const uint32_t off1 = cb.pushPayload( d );

	A16			   v { 1, 2, 3, 4 };
	const uint32_t before2 = static_cast<uint32_t>( cb.payload.size() );
	const uint32_t off2	   = cb.pushPayload( v );

	REQUIRE( ( off2 % alignof( A16 ) ) == 0 );
	REQUIRE( off2 + sizeof( A16 ) <= cb.payload.size() );

	// Check padding.
	for ( uint32_t i = before2; i < off2; ++i )
	{
		REQUIRE( cb.payload[ i ] == std::byte { 0 } );
	}

	const auto & back = cb.getPayload<A16>( off2 );
	REQUIRE( bytesEqual( &back, &v, sizeof( A16 ) ) );
}

TEST_CASE( "CommandBuffer: clear empties commands and payload" )
{
	CommandBuffer cb;
	cb.push<E_COMMAND::CLEAR>();
	cb.push<E_COMMAND::SET_VIEWPORT>( PayloadViewport { 1, 2, 3, 4 } );

	REQUIRE( not cb.commands.empty() );
	REQUIRE( not cb.payload.empty() );

	cb.clear();

	REQUIRE( cb.commands.empty() );
	REQUIRE( cb.payload.empty() );
}
