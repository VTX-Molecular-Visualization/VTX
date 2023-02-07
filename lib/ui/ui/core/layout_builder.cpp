#include "layout_builder.hpp"
#include "core/tool_registry.hpp"
#include <old/tool/logger.hpp>

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
		BaseVTXUITool * const tool = UI::Core::ToolRegistry::createInstance( p_toolDescriptor.getIdentifier() );

		if ( tool == nullptr )
		{
			VTX_ERROR( "Tool " + p_toolDescriptor.getIdentifier() + " not loaded into VTX." );
			return;
		}

		tool->setLayoutData( p_toolDescriptor.getLayoutData() );
		tool->instantiateTool();
	}

} // namespace VTX::UI::Core
