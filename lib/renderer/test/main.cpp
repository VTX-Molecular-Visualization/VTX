#include <catch2/catch_test_macros.hpp>
#include <renderer/context/opengl_45.hpp>
#include <renderer/render_graph.hpp>
#include <renderer/scheduler/depth_first_search.hpp>

// filesystem.hpp
TEST_CASE( "Renderer::RenderGraph", "[renderer]" )
{
	using namespace VTX::Renderer;
	RenderGraph<Context::OpenGL45, Scheduler::DepthFirstSearch> graph;

	// Passes.
	graph.addPass( "Shading",
				   { Pass::Inputs { { E_INPUT_CHANNEL::COLOR_0, { "G" } },

									{ E_INPUT_CHANNEL::DEPTH, { "D" } } } } );
	graph.addPass( "Geometric", {} );
	graph.addPass( "FXAA", { Pass::Inputs { { E_INPUT_CHANNEL::COLOR_0, { "S" } } } } );

	graph.addPass( "Depth",
				   { Pass::Inputs { { E_INPUT_CHANNEL::COLOR_0, { "G" } }, { E_INPUT_CHANNEL::COLOR_1, { "T" } } } } );

	// Links.
	graph.addLink( "Geometric", "Depth", E_INPUT_CHANNEL::COLOR_0 );
	graph.addLink( "Geometric", "Shading", E_INPUT_CHANNEL::COLOR_0 );
	graph.addLink( "Depth", "Shading", E_INPUT_CHANNEL::DEPTH );
	graph.addLink( "Shading", "FXAA", E_INPUT_CHANNEL::COLOR_0 );

	REQUIRE( graph.setup() );

	// Connect channel already in used.
	REQUIRE_FALSE( graph.addLink( "Geometric", "FXAA", E_INPUT_CHANNEL::COLOR_0 ) );

	// Add cyclic link.
	graph.addLink( "FXAA", "Depth", E_INPUT_CHANNEL::COLOR_1 );

	REQUIRE_FALSE( graph.setup() );
}
