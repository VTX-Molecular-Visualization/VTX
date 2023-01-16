#include "vtx_layout_reader.hpp"
#include "__new_archi/ui/default_tools/keys.hpp"

namespace VTX::UI::Core::IO
{
	void VTXLayoutReader::read()
	{
		_result = Result();

		ToolLayoutData layoutData;

		_result.layoutDescriptor.addTool( ToolDescriptor( VTX::UI::DefaultTools::RENDER_WINDOW_KEY ) );
		_result.layoutDescriptor.addTool( ToolDescriptor( VTX::UI::DefaultTools::SCENE_WINDOW_KEY ) );

		layoutData			 = UI::Core::ToolLayoutData();
		layoutData.tabName	 = "Main";
		layoutData.blockName = "File";
		_result.layoutDescriptor.addTool( ToolDescriptor( VTX::UI::DefaultTools::MOLECULE_LOADER_KEY, layoutData ) );

		layoutData			 = UI::Core::ToolLayoutData();
		layoutData.tabName	 = "Tools";
		layoutData.blockName = "Structural Alignment";

		_result.layoutDescriptor.addTool( ToolDescriptor( "RMSDTool", layoutData ) );

		layoutData			 = UI::Core::ToolLayoutData();
		layoutData.tabName	 = "Tools";
		layoutData.blockName = "Structural Alignment";

		_result.layoutDescriptor.addTool( ToolDescriptor( "StructuralAlignmentTool", layoutData ) );
	}

} // namespace VTX::UI::Core::IO
