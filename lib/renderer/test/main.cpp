#include <catch2/catch_test_macros.hpp>
#include <renderer/render_graph.hpp>
#include <renderer/renderer.hpp>

TEST_CASE( "Renderer::RenderGraph", "[renderer]" )
{
	using namespace VTX;
	using namespace VTX::Renderer;

	RenderGraph graph;

	/*
	Pass A = { "A" };
	A.inputs.emplace( E_CHAN_IN::_0, Input { "AI0", {} } );
	A.outputs.emplace( E_CHAN_OUT::COLOR_0, Output { "AO0", {} } );

	Pass B = { "B" };
	B.inputs.emplace( E_CHAN_IN::_0, Input { "BI0", {} } );
	B.outputs.emplace( E_CHAN_OUT::COLOR_0, Output { "BO0", {} } );

	Pass C = { "C" };
	C.inputs.emplace( E_CHAN_IN::_0, Input { "CI0", {} } );
	C.outputs.emplace( E_CHAN_OUT::COLOR_0, Output { "CO0", {} } );

	Pass * passA = graph.addPass( A );
	Pass * passB = graph.addPass( B );
	Pass * passC = graph.addPass( C );
	graph.addLink( passA, passB, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
	graph.addLink( passB, passC, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );

	// No output exception.
	REQUIRE_THROWS( graph.build<Scheduler::DepthFirstSearch>() );

	graph.setOutput( &passC->outputs.at( E_CHAN_OUT::COLOR_0 ) );

	RenderQueue queue = graph.build<Scheduler::DepthFirstSearch>();

	// Expected order: A -> B -> C.
	REQUIRE( queue.size() == 3 );
	REQUIRE( queue[ 0 ]->name == "A" );
	REQUIRE( queue[ 1 ]->name == "B" );
	REQUIRE( queue[ 2 ]->name == "C" );

	Pass D = { "D" };
	D.inputs.emplace( E_CHAN_IN::_0, Input { "DI0", {} } );
	D.outputs.emplace( E_CHAN_OUT::COLOR_0, Output { "DO0", {} } );
	Pass * passD = graph.addPass( D );

	queue = graph.build<Scheduler::DepthFirstSearch>();

	// Expected order: A -> B -> C (D is not connected).
	REQUIRE( queue.size() == 3 );
	REQUIRE( queue[ 0 ]->name == "A" );
	REQUIRE( queue[ 1 ]->name == "B" );
	REQUIRE( queue[ 2 ]->name == "C" );

	graph.addLink( passD, passA, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
	queue = graph.build<Scheduler::DepthFirstSearch>();

	// Expected order: D -> A -> B -> C.
	REQUIRE( queue.size() == 4 );
	REQUIRE( queue[ 0 ]->name == "D" );
	REQUIRE( queue[ 1 ]->name == "A" );
	REQUIRE( queue[ 2 ]->name == "B" );
	REQUIRE( queue[ 3 ]->name == "C" );

	graph.removePass( passB );
	graph.addLink( passA, passC, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );
	queue = graph.build<Scheduler::DepthFirstSearch>();

	// Expected order: D -> A -> C.
	REQUIRE( queue.size() == 3 );
	REQUIRE( queue[ 0 ]->name == "D" );
	REQUIRE( queue[ 1 ]->name == "A" );
	REQUIRE( queue[ 2 ]->name == "C" );

	// Add loop.
	graph.addLink( passA, passD, E_CHAN_OUT::COLOR_0, E_CHAN_IN::_0 );

	// Cyclic graph exception.
	REQUIRE_THROWS( graph.build<Scheduler::DepthFirstSearch>() );
	*/
}

TEST_CASE( "Renderer::Context::Opengl45", "[renderer]" )
{
	using namespace VTX::Renderer;
	using namespace VTX::Util;

	VTX::Renderer::Renderer renderer( 800, 600 );

	// renderer.setDefault();

	renderer.resize( 1024, 768 );
	// TODO: test buffer sizes and others things.
}
