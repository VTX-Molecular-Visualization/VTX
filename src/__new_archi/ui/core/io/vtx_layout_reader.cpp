#include "vtx_layout_reader.hpp"

namespace VTX::UI::Core::IO
{
	void VTXLayoutReader::read()
	{
		_result = Result();

		ToolLayoutData layoutData;

		layoutData			 = UI::Core::ToolLayoutData();
		layoutData.tabName	 = "Tools";
		layoutData.blockName = "RMSD";

		_result.layoutDescriptor.addTool( ToolDescriptor( "RMSDTool", layoutData ) );

		layoutData			 = UI::Core::ToolLayoutData();
		layoutData.tabName	 = "Tools";
		layoutData.blockName = "Structural Alignment";

		_result.layoutDescriptor.addTool( ToolDescriptor( "StructuralAlignmentTool", layoutData ) );
	}

} // namespace VTX::UI::Core::IO
