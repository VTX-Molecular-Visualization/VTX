#include <catch2/catch_test_macros.hpp>
#include <renderer/render_graph.hpp>
#include <renderer/renderer.hpp>
#include <renderer/scheduler/depth_first_search.hpp>

TEST_CASE( "Renderer::RenderGraph", "[renderer]" )
{
	using namespace VTX;
	using namespace VTX::Renderer;

	RenderGraph graph;

	Pass A	 = { "A" };
	A.inputs = {};
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

	graph.setOutput( &passC->outputs.at( E_CHAN_OUT::COLOR_0 ) );

	graph.build<Scheduler::DepthFirstSearch>();

	const RenderQueue & queue = graph.getRenderQueue();
	REQUIRE( queue.size() == 3 );
	REQUIRE( queue[ 0 ]->name == "A" );
	REQUIRE( queue[ 1 ]->name == "B" );
	REQUIRE( queue[ 2 ]->name == "C" );
}

TEST_CASE( "Renderer::Context::Opengl45", "[renderer]" )
{
	using namespace VTX::Renderer;
	using namespace VTX::Util;

	VTX::Renderer::Renderer renderer( 800, 600 );

	renderer.setDefault();

	renderer.resize( 1024, 768 );
	// TODO: test buffer sizes and others things.
}
