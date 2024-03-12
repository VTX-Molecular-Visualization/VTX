#include "ui/core/io/vtx_layout_reader.hpp"

namespace VTX::UI::Core::IO
{
	void VTXLayoutReader::read()
	{
		_result = Result();

		// ToolLayoutData layoutData;
		_result.layoutDescriptor.addTool( ToolDescriptor( "RENDER_WINDOW_KEY" ) );
		_result.layoutDescriptor.addTool( ToolDescriptor( "CONSOLE_TOOL_KEY" ) );
		_result.layoutDescriptor.addTool( ToolDescriptor( "UI_FEATURE_QUIT" ) );
		_result.layoutDescriptor.addTool( ToolDescriptor( "MISCELLANEOUS_ORIENT_ON_FIRST_MOLECULE_LOADED" ) );
	}

} // namespace VTX::UI::Core::IO
