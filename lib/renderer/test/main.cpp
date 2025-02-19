#include <catch2/catch_test_macros.hpp>
#include <renderer/facade.hpp>
#include <util/filesystem.hpp>

TEST_CASE( "Renderer::RenderGraph", "[renderer]" )
{
	// TODO: redo with new architecture and fixed scheduler.

	using namespace VTX::Renderer;

	// 	RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch> _graph;
	//
	// 	// Passes.
	// 	_graph.addPass( "Shading",
	// 				   { Pass::Inputs { { E_CHANNEL::COLOR_0, { "G" } },
	//
	// 									{ E_CHANNEL::DEPTH, { "D" } } } } );
	// 	_graph.addPass( "Geometric", {} );
	// 	_graph.addPass( "FXAA", { Pass::Inputs { { E_CHANNEL::COLOR_0, { "S" } } } } );
	//
	// 	_graph.addPass( "Depth", { Pass::Inputs { { E_CHANNEL::COLOR_0, { "G" } }, { E_CHANNEL::COLOR_1, { "T" } } } } );
	//
	// 	// Links.
	// 	_graph.addLink( "Geometric", "Depth", E_CHANNEL::COLOR_0 );
	// 	_graph.addLink( "Geometric", "Shading", E_CHANNEL::COLOR_0 );
	// 	_graph.addLink( "Depth", "Shading", E_CHANNEL::DEPTH );
	// 	_graph.addLink( "Shading", "FXAA", E_CHANNEL::COLOR_0 );
	//
	// 	REQUIRE( _graph.setup() );
	//
	// 	// Connect channel already in used.
	// 	REQUIRE_FALSE( _graph.addLink( "Geometric", "FXAA", E_CHANNEL::COLOR_0 ) );
	//
	// 	// Add cyclic link.
	// 	_graph.addLink( "FXAA", "Depth", E_CHANNEL::COLOR_1 );
	//
	// 	REQUIRE_FALSE( _graph.setup() );
}

TEST_CASE( "Renderer::Context::Opengl45", "[renderer]" )
{
	using namespace VTX::Renderer;
	using namespace VTX::Util;

	Facade renderer( 800, 600 );

	renderer.setDefault();

	renderer.resize( 1024, 768 );
	// TODO: test buffer sizes and others things.
}
