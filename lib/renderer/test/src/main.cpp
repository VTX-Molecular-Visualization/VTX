#include <catch2/catch_test_macros.hpp>
#include <renderer/renderer.hpp>

TEST_CASE( "Renderer::RenderGraph", "[renderer]" )
{
	// TODO: redo with new architecture and fixed scheduler.

	using namespace VTX::Renderer;

	// 	RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch> graph;
	//
	// 	// Passes.
	// 	graph.addPass( "Shading",
	// 				   { Pass::Inputs { { E_CHANNEL::COLOR_0, { "G" } },
	//
	// 									{ E_CHANNEL::DEPTH, { "D" } } } } );
	// 	graph.addPass( "Geometric", {} );
	// 	graph.addPass( "FXAA", { Pass::Inputs { { E_CHANNEL::COLOR_0, { "S" } } } } );
	//
	// 	graph.addPass( "Depth", { Pass::Inputs { { E_CHANNEL::COLOR_0, { "G" } }, { E_CHANNEL::COLOR_1, { "T" } } } } );
	//
	// 	// Links.
	// 	graph.addLink( "Geometric", "Depth", E_CHANNEL::COLOR_0 );
	// 	graph.addLink( "Geometric", "Shading", E_CHANNEL::COLOR_0 );
	// 	graph.addLink( "Depth", "Shading", E_CHANNEL::DEPTH );
	// 	graph.addLink( "Shading", "FXAA", E_CHANNEL::COLOR_0 );
	//
	// 	REQUIRE( graph.setup() );
	//
	// 	// Connect channel already in used.
	// 	REQUIRE_FALSE( graph.addLink( "Geometric", "FXAA", E_CHANNEL::COLOR_0 ) );
	//
	// 	// Add cyclic link.
	// 	graph.addLink( "FXAA", "Depth", E_CHANNEL::COLOR_1 );
	//
	// 	REQUIRE_FALSE( graph.setup() );
}

TEST_CASE( "Renderer::Context::Opengl45", "[renderer]" )
{
	using namespace VTX::Renderer;

	Renderer renderer( 800, 600, "" );
	// Verify that the context is not created (not possible without loader).
	REQUIRE_THROWS( renderer.build() );

	// TODO: test buffer sizes and others things.
}
