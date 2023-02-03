#include "layout_builder.hpp"
#include "__new_archi/ui/core/tool_registry.hpp"

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
		tool->setLayoutData( p_toolDescriptor.getLayoutData() );

		if ( tool != nullptr )
			tool->instantiateTool();
	}

} // namespace VTX::UI::Core
