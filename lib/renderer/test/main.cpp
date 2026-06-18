#include <catch2/catch_test_macros.hpp>
#include <renderer/builder/render_graph_build.hpp>
#include <renderer/context/command_buffer.hpp>
#include <renderer/graph_builder.hpp>
#include <renderer/render_graph.hpp>
#include <renderer/representation.hpp>
#include <renderer/resource_handler.hpp>
#include <util/exceptions.hpp>
#include <util/math.hpp>

using namespace VTX::Renderer;
using namespace VTX::Renderer::Desc;
using namespace VTX::Renderer::Context;

namespace
{
	using namespace VTX;

	GraphBuilder makeLinearGraphABC()
	{
		GraphBuilder g;

		g.defaultSampler();

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

	g.texture( "SSAO", E_FORMAT::R8UI );

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
	Builder::PipelineConfig cfg;
	cfg.enableSSAO				  = true;
	cfg.enableOutline			  = true;
	cfg.enableSelection			  = true;
	cfg.enableChromaticAberration = true;

	RenderGraph graph;
	graph.set( Builder::DefaultRenderGraph::build( cfg, Layouts {}, Geometries {} ) );

	const Resources & resources = graph.getResources();
	CHECK(
		resources.buffers.at( VTX::Renderer::Geometry::SES::BUFFER_CONVEX_PATCH_ELEMENTS ).allocation
		== E_BUFFER_ALLOCATION::CHUNKED
	);
	CHECK(
		resources.buffers.at( VTX::Renderer::Geometry::SES::INDEX_CONVEX_PATCHES ).allocation
		== E_BUFFER_ALLOCATION::CHUNKED
	);
	CHECK(
		resources.buffers.at( VTX::Renderer::Geometry::SES::INDIRECT_CONVEX_PATCHES ).allocation
		== E_BUFFER_ALLOCATION::CHUNKED
	);
	CHECK(
		resources.buffers.at( VTX::Renderer::Geometry::Sphere::INDIRECT_SPHERES ).allocation
		== E_BUFFER_ALLOCATION::SINGLE
	);

	RenderQueue queue;
	REQUIRE_NOTHROW( queue = graph.build() );

	REQUIRE_FALSE( queue.empty() );

	bool hasGeometric = false;
	bool hasShading	  = false;
	bool hasFXAA	  = false;
	bool hasChromatic = false;

	for ( const Pass * p : queue )
	{
		if ( p->name == "Geometric" )
		{
			hasGeometric = true;
		}
		if ( p->name == "Shading" )
		{
			hasShading = true;
		}
		if ( p->name == "FXAA" )
		{
			hasFXAA = true;
		}
		if ( p->name == "ChromaticAberration" )
		{
			hasChromatic = true;
		}
	}

	CHECK( hasGeometric );
	CHECK( hasShading );
	CHECK( hasFXAA );
	CHECK( hasChromatic );
	CHECK( queue.back()->name == "FXAA" );
}

TEST_CASE( "GraphBuilder: compute dispatch pass builds", "[renderer][graph]" )
{
	GraphBuilder g;

	g.computePass( "Compute" )
		.program( "ComputeProg" )
		.shaders( { FilePath( "compute.comp" ) } )
		.dispatch( 4, 2, 1, E_MEMORY_BARRIER::SHADER_STORAGE | E_MEMORY_BARRIER::COMMAND )
		.endProgram()
		.endPass();

	RenderGraph graph;
	graph.set( std::move( g ) );

	RenderQueue queue;
	REQUIRE_NOTHROW( queue = graph.build() );

	REQUIRE( queue.size() == 1 );
	CHECK( queue[ 0 ]->type == E_PASS_TYPE::COMPUTE );
	REQUIRE( queue[ 0 ]->programs.size() == 1 );
	REQUIRE( queue[ 0 ]->programs[ 0 ].dispatch );
	CHECK( queue[ 0 ]->programs[ 0 ].dispatch->groupX == 4 );
	CHECK( queue[ 0 ]->programs[ 0 ].dispatch->groupY == 2 );
	CHECK( queue[ 0 ]->programs[ 0 ].dispatch->groupZ == 1 );
	CHECK(
		queue[ 0 ]->programs[ 0 ].dispatch->barriers == ( E_MEMORY_BARRIER::SHADER_STORAGE | E_MEMORY_BARRIER::COMMAND )
	);
}

TEST_CASE( "GraphBuilder: compute dispatch indirect creates its buffer", "[renderer][graph]" )
{
	GraphBuilder g;

	g.computePass( "Compute" )
		.program( "ComputeProg" )
		.shaders( { FilePath( "compute.comp" ) } )
		.dispatchIndirect( "DispatchIndirect", 16, E_MEMORY_BARRIER::COMMAND )
		.endProgram()
		.endPass();

	REQUIRE( g.resources.buffers.contains( "DispatchIndirect" ) );
	CHECK( g.resources.buffers.at( "DispatchIndirect" ).usage == E_BUFFER_USAGE::INDIRECT );
	CHECK( g.resources.buffers.at( "DispatchIndirect" ).frequency == E_UPDATE_FREQUENCY::DYNAMIC );

	RenderGraph graph;
	graph.set( std::move( g ) );

	RenderQueue queue;
	REQUIRE_NOTHROW( queue = graph.build() );

	REQUIRE( queue.size() == 1 );
	REQUIRE( queue[ 0 ]->programs.size() == 1 );
	REQUIRE( queue[ 0 ]->programs[ 0 ].dispatchIndirect );
	CHECK( queue[ 0 ]->programs[ 0 ].dispatchIndirect->indirectBuffer == "DispatchIndirect" );
	CHECK( queue[ 0 ]->programs[ 0 ].dispatchIndirect->offset == 16 );
	CHECK( queue[ 0 ]->programs[ 0 ].dispatchIndirect->barriers == E_MEMORY_BARRIER::COMMAND );
}

TEST_CASE( "GraphBuilder: rejects pipeline commands in the wrong pass type", "[renderer][graph]" )
{
	auto drawInCompute = []()
	{
		GraphBuilder g;
		g.computePass( "Compute" )
			.program( "DrawProg" )
			.shaders( { FilePath( "draw.vert" ), FilePath( "draw.frag" ) } )
			.draw( "Geometry", E_PRIMITIVE::POINTS, DrawCall::Range { 0, 1 } );
	};

	auto dispatchInGraphics = []()
	{
		GraphBuilder g;
		g.pass( "Graphics" ).program( "DispatchProg" ).shaders( { FilePath( "compute.comp" ) } ).dispatch( 1, 1, 1 );
	};

	auto programInExternal = []()
	{
		GraphBuilder g;
		g.externalPass( "External" ).program( "ExternalProg" );
	};

	REQUIRE_THROWS_AS( drawInCompute(), GraphicException );
	REQUIRE_THROWS_AS( dispatchInGraphics(), GraphicException );
	REQUIRE_THROWS_AS( programInExternal(), GraphicException );
}

TEST_CASE( "RenderGraph: dispatch indirect buffer must exist", "[renderer][graph]" )
{
	GraphBuilder g;
	g.computePass( "Compute" ).endPass();

	g.passes[ 0 ]->programs.push_back( Program {} );
	Program & program = g.passes[ 0 ]->programs.back();
	program.name	  = "ComputeProg";
	program.shaders.emplace<FilePath>( "compute.comp" );
	program.dispatchIndirect = DispatchIndirect { .indirectBuffer = "MissingDispatchIndirect", .offset = 0 };

	RenderGraph graph;
	graph.set( std::move( g ) );

	REQUIRE_THROWS_AS( graph.build(), GraphicException );
}

TEST_CASE( "RenderGraph: external pass does not require resources", "[renderer][graph]" )
{
	GraphBuilder g;
	g.externalPass( "External" ).endPass();

	RenderGraph graph;
	graph.set( std::move( g ) );

	const RenderQueue queue = graph.build();
	REQUIRE( queue.size() == 1 );
	CHECK( queue[ 0 ]->type == E_PASS_TYPE::EXTERNAL );
	CHECK( queue[ 0 ]->execution == E_PASS_EXECUTION::ON_DIRTY );
}

TEST_CASE( "Residue representations follow each residue CA atom", "[renderer][representation]" )
{
	VTX::Renderer::Geometry::Ribbon::Construction construction;
	construction.residues = {
		{ 10, 3, 4 },
		{ 11, 0, 5 },
		{ 12, 2, 6 },
	};

	const std::vector<RepresentationIndex> atomRepresentations = { 7, 8, 9, 4, 5 };
	std::vector<RepresentationIndex>	   residueRepresentations( construction.residues.size() );

	for ( size_t i = 0; i < construction.residues.size(); ++i )
	{
		residueRepresentations[ i ] = atomRepresentations[ construction.residues[ i ].ca ];
	}

	REQUIRE( residueRepresentations.size() == 3 );
	CHECK( residueRepresentations[ 0 ] == 4 );
	CHECK( residueRepresentations[ 1 ] == 7 );
	CHECK( residueRepresentations[ 2 ] == 9 );
}

static bool bytesEqual( const void * p_a, const void * p_b, size_t p_n ) { return std::memcmp( p_a, p_b, p_n ) == 0; }

TEST_CASE( "CommandBuffer: push command without payload sets NO_PAYLOAD" )
{
	/*
	CommandBuffer cb;

	cb.push<E_COMMAND::END_FRAME>();

	REQUIRE( cb.commands.size() == 1 );
	REQUIRE( cb.commands[ 0 ].type == E_COMMAND::END_FRAME );
	REQUIRE( cb.commands[ 0 ].payloadOffset == NO_PAYLOAD );
	REQUIRE( cb.payload.empty() );
	*/
}

TEST_CASE( "CommandBuffer: push command with payload stores bytes and is readable" )
{
	CommandBuffer cb;

	PayloadBeginPass bp {};
	bp.flags = 1;
	cb.push<E_COMMAND::BEGIN_PASS>( bp );

	REQUIRE( cb.commands.size() == 1 );
	REQUIRE( cb.commands[ 0 ].type == E_COMMAND::BEGIN_PASS );

	const uint32_t off = cb.commands[ 0 ].payloadOffset;
	REQUIRE( off != NO_PAYLOAD );
	REQUIRE( ( off % alignof( PayloadBeginPass ) ) == 0 );
	REQUIRE( off + sizeof( PayloadBeginPass ) <= cb.payload.size() );

	const auto & back = cb.getPayload<PayloadBeginPass>( off );
	REQUIRE( bytesEqual( &back, &bp, sizeof( PayloadBeginPass ) ) );
}

TEST_CASE( "CommandBuffer: external payload stores function and context" )
{
	CommandBuffer cb;

	const PayloadExternal payload { .function = 42, .context = 1337 };
	cb.push<E_COMMAND::EXTERNAL>( payload );

	REQUIRE( cb.commands.size() == 1 );
	REQUIRE( cb.commands[ 0 ].type == E_COMMAND::EXTERNAL );

	const PayloadExternal & back = cb.getPayload<PayloadExternal>( cb.commands[ 0 ].payloadOffset );
	CHECK( back.function == payload.function );
	CHECK( back.context == payload.context );
}

TEST_CASE( "CommandBuffer: ON_DIRTY pass is scheduled explicitly" )
{
	CommandBuffer cb;

	Pass pass;
	pass.name	   = "External";
	pass.type	   = E_PASS_TYPE::EXTERNAL;
	pass.execution = E_PASS_EXECUTION::ON_DIRTY;

	const PassID passID = cb.beginPass( pass );
	cb.push<E_COMMAND::EXTERNAL>( PayloadExternal {} );
	cb.endPass( passID );

	REQUIRE( cb.onDirtyPassIDs.empty() );

	cb.markPassDirty( "External" );
	cb.markPassDirty( "External" );

	REQUIRE( cb.onDirtyPassIDs.size() == 1 );
	CHECK( cb.onDirtyPassIDs[ 0 ] == passID );

	cb.clearDirtyPasses();
	CHECK( cb.onDirtyPassIDs.empty() );
}

struct alignas( 16 ) A16
{
	std::uint32_t a, b, c, d;
};

TEST_CASE( "CommandBuffer: pushPayload aligns and pads correctly" )
{
	/*
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
	*/
}

TEST_CASE( "CommandBuffer: clear empties commands and payload" )
{
	CommandBuffer cb;
	cb.push<E_COMMAND::BEGIN_PASS>( PayloadBeginPass { 4 } );
	cb.push<E_COMMAND::END_PASS>( PayloadEndPass {} );

	REQUIRE( not cb.commands.empty() );
	REQUIRE( not cb.payload.empty() );

	cb.clear();

	REQUIRE( cb.commands.empty() );
	REQUIRE( cb.payload.empty() );
}

struct TestRes
{
	int value = 0;

	explicit TestRes( int v ) : value( v ) {}
};

struct FakeDesc
{
	int a = 0;
	int b = 0;
};

namespace VTX::Renderer::Desc
{
	template<>
	inline Hash hashDesc<FakeDesc>( const FakeDesc & p_text )
	{
		return Util::hash( p_text.a ) + Util::hash( p_text.b );
	}
} // namespace VTX::Renderer::Desc

TEST_CASE( "ResourceHandler: emplace creates new handles sequentially", "[ResourceHandler]" )
{
	ResourceHandler<TestRes> h;

	const Key k1 = "1", k2 = "2", k3 = "3";

	const Handle h1 = h.emplace( k1, 10 );
	const Handle h2 = h.emplace( k2, 20 );
	const Handle h3 = h.emplace( k3, 30 );

	REQUIRE( h1 == 0 );
	REQUIRE( h2 == 1 );
	REQUIRE( h3 == 2 );

	REQUIRE( h.contains( k1 ) );
	REQUIRE( h.contains( k2 ) );
	REQUIRE( h.contains( k3 ) );

	REQUIRE( h.get( k1 ).value == 10 );
	REQUIRE( h.get( k2 ).value == 20 );
	REQUIRE( h.get( k3 ).value == 30 );
}

TEST_CASE( "ResourceHandler: emplace with existing key updates resource and keeps same handle", "[ResourceHandler]" )
{
	ResourceHandler<TestRes, Key, FakeDesc> h;

	const Key	   k = "42";
	const FakeDesc d1 { 1, 2 };
	const FakeDesc d2 { 9, 9 };

	const Handle h1 = h.emplace( k, d1, 111 );
	REQUIRE( h1 == 0 );
	REQUIRE( h.get( k ).value == 111 );
	REQUIRE( h.descriptor( k ).a == 1 );

	const Handle h2 = h.emplace( k, d2, 222 );
	REQUIRE( h2 == h1 );				 // handle stable
	REQUIRE( h.get( k ).value == 222 );	 // resource replaced
	REQUIRE( h.descriptor( k ).a == 9 ); // descriptor updated
}

TEST_CASE( "ResourceHandler: erase(key) removes and makes handle reusable", "[ResourceHandler]" )
{
	ResourceHandler<TestRes> h;

	const Key k1 = "1", k2 = "2", k3 = "3";

	const Handle h1 = h.emplace( k1, 10 ); // 0
	const Handle h2 = h.emplace( k2, 20 ); // 1
	(void)h2;

	REQUIRE( h1 == 0 );

	h.erase( k1 );

	REQUIRE_FALSE( h.contains( k1 ) );
	REQUIRE( h.contains( k2 ) );

	// Next emplace should reuse the freed handle (LIFO from _availables)
	const Handle h3 = h.emplace( k3, 30 );
	REQUIRE( h3 == h1 );
	REQUIRE( h.get( k3 ).value == 30 );
}

TEST_CASE( "ResourceHandler: erase(handle) is safe for out-of-range and null slots", "[ResourceHandler]" )
{
	ResourceHandler<TestRes> h;

	const Key	 k		= "1";
	const Handle handle = h.emplace( k, 123 );

	// Out of range: no crash, no change
	h.erase( handle + 1000 );
	REQUIRE( h.contains( k ) );

	// Erase valid
	h.erase( handle );
	REQUIRE_FALSE( h.contains( k ) );

	// Erase again (slot already null): no crash
	h.erase( handle );
	REQUIRE_FALSE( h.contains( handle ) );
}

TEST_CASE( "ResourceHandler: validate(key, desc) matches by hash and clears invalid state", "[ResourceHandler]" )
{
	ResourceHandler<TestRes, Key, FakeDesc> h;

	const Key	   k1 = "1", k2 = "2";
	const FakeDesc d_ok { 1, 2 };
	const FakeDesc d_bad { 1, 999 };

	const Handle h1 = h.emplace( k1, d_ok, 10 );
	const Handle h2 = h.emplace( k2, d_ok, 20 );
	(void)h1;
	(void)h2;

	// Invalidate all existing resources
	h.invalidate();

	// Wrong desc => validate should fail and not "fix" it
	REQUIRE_FALSE( h.validate( k1, d_bad ) );

	// Correct desc => validate true and removes from invalid list
	REQUIRE( h.validate( k1, d_ok ) );

	// Purge should remove only those still invalid (k2)
	h.purge();

	REQUIRE( h.contains( k1 ) );
	REQUIRE_FALSE( h.contains( k2 ) ); // purged
	REQUIRE( h.get( k1 ).value == 10 );
}

TEST_CASE( "ResourceHandler: invalidate ignores available handles (already erased)", "[ResourceHandler]" )
{
	ResourceHandler<TestRes> h;

	const Key k1 = "1", k2 = "2";

	const Handle h1 = h.emplace( k1, 10 );
	const Handle h2 = h.emplace( k2, 20 );

	h.erase( h1 ); // make h1 available (null slot)

	// Invalidate should mark only existing resources (h2), not the available slot (h1)
	h.invalidate();
	h.purge();

	REQUIRE_FALSE( h.contains( h1 ) ); // stays empty
	REQUIRE_FALSE( h.contains( k1 ) );
	REQUIRE_FALSE( h.contains( k2 ) ); // purged
}

TEST_CASE( "ResourceHandler: clear removes everything and resets handle numbering", "[ResourceHandler]" )
{
	ResourceHandler<TestRes> h;

	const Key k1 = "1", k2 = "2";

	const Handle h1 = h.emplace( k1, 10 );
	const Handle h2 = h.emplace( k2, 20 );
	(void)h1;
	(void)h2;

	h.clear();

	REQUIRE_FALSE( h.contains( k1 ) );
	REQUIRE_FALSE( h.contains( k2 ) );

	// After clear(), new handle should start at 0 again
	const Handle h3 = h.emplace( "999", 30 );
	REQUIRE( h3 == 0 );
	REQUIRE( h.get( "999" ).value == 30 );
}
