#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <tool/topology_editor/topology_editor.hpp>

int main( int argc, char * argv[] )
{
	return Catch::Session().run( argc, argv );
}

TEST_CASE( "TopologyEditor can be constructed", "[topology_editor]" )
{
	VTX::Tool::TopologyEditor::TopologyEditor tool;
	CHECK( true );
}
