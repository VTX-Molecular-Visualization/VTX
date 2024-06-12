#include "tool/core/layout_builder.hpp"
#include "tool/core/tool_handler.hpp"
#include <util/logger.hpp>

namespace VTX::UI::Core
{
	void LayoutBuilder::build( const LayoutDescriptor & p_layoutDescriptor ) const
	{
		for ( const ToolDescriptor & toolDescriptor : p_layoutDescriptor.getTools() )
		{
			_instantiateTool( toolDescriptor );
		}
	}

	void LayoutBuilder::_instantiateTool( const ToolDescriptor & p_toolDescriptor ) const
	{
		BaseVTXUITool * toolPtr = TOOL_HANDLER().create( p_toolDescriptor.getIdentifier() );

		if ( toolPtr == nullptr )
		{
			VTX_ERROR( "Tool {} not loaded into VTX.", p_toolDescriptor.getIdentifier() );
			return;
		}

		toolPtr->setLayoutData( p_toolDescriptor.getLayoutData() );
		toolPtr->instantiateTool();
	}

} // namespace VTX::UI::Core
