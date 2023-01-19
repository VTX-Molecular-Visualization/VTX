#include "vtx_layout_reader.hpp"
#include "__new_archi/ui/qt/tool/keys.hpp"

namespace VTX::UI::Core::IO
{
	void VTXLayoutReader::read()
	{
		_result = Result();

		ToolLayoutData layoutData;

		_result.layoutDescriptor.addTool( ToolDescriptor( VTX::UI::QT::Tool::RENDER_WINDOW_KEY ) );
		_result.layoutDescriptor.addTool( ToolDescriptor( VTX::UI::QT::Tool::SCENE_WINDOW_KEY ) );

		layoutData			 = UI::Core::ToolLayoutData();
		layoutData.tabName	 = "Main";
		layoutData.blockName = "File";
		_result.layoutDescriptor.addTool( ToolDescriptor( VTX::UI::QT::Tool::SESSION_TOOLS_KEY, layoutData ) );

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
