#include "tool/core/layout_descriptor.hpp"

namespace VTX::UI::Core
{

	ToolDescriptor::ToolDescriptor() : ToolDescriptor( UNKNOWN_TOOL_IDENTIFIER ) {}
	ToolDescriptor::ToolDescriptor( const ToolIdentifier & p_identifier ) : _identifier( p_identifier ) {}
	ToolDescriptor::ToolDescriptor( const ToolIdentifier & p_identifier, const ToolLayoutData p_layoutData ) :
		_identifier( p_identifier ), _layoutData( p_layoutData )
	{
	}

	void LayoutDescriptor::addTool( const ToolDescriptor & p_toolDescriptor )
	{
		_tools.emplace_back( p_toolDescriptor );
	}

} // namespace VTX::UI::Core
