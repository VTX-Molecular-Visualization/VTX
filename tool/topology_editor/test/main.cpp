#include <catch2/catch_test_macros.hpp>
#include <tool/topology_editor/topology_editor.hpp>

TEST_CASE( "TopologyEditor can be constructed", "[topology_editor]" )
{
	VTX::Tool::TopologyEditor::TopologyEditor tool;
	CHECK( true );
}
