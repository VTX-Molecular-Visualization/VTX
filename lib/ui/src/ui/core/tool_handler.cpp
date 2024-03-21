#include "ui/core/tool_handler.hpp"
#include "ui/core/tool_registry.hpp"
#include "ui/qt/widget/custom/dock_window_main_widget.hpp"

namespace VTX::UI::Core
{
	ToolHandler::ToolHandler() {}
	ToolHandler::~ToolHandler() {}

	BaseVTXUITool * ToolHandler::create( const ToolIdentifier & p_identifier )
	{
		assert( !exists( p_identifier ) );

		if ( !ToolRegistry::exists( p_identifier ) )
			return nullptr;

		const std::unique_ptr<BaseVTXUITool> & toolPtr
			= _storedTools.emplace_back( ToolRegistry::createInstance( p_identifier ) );

		_mapTools[ p_identifier ] = toolPtr.get();

		return toolPtr.get();
	}

	void ToolHandler::link( const ToolIdentifier & p_identifier, BaseVTXUITool * p_toolPtr )
	{
		assert( !exists( p_identifier ) );
		_mapTools[ p_identifier ] = p_toolPtr;
	}

	BaseVTXUITool & ToolHandler::get( const ToolIdentifier & p_identifier ) const
	{
		assert( exists( p_identifier ) );
		return *( _mapTools.at( p_identifier ) );
	}
	bool ToolHandler::exists( const ToolIdentifier & p_identifier ) const { return _mapTools.contains( p_identifier ); }

	ToolHandler & TOOL_HANDLER() { return ToolHandler::SYSTEM.get(); }

} // namespace VTX::UI::Core
