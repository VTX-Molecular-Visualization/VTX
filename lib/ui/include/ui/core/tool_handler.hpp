#ifndef __VTX_UI_CORE_TOOL_HANDLER__
#define __VTX_UI_CORE_TOOL_HANDLER__

#include <app/application/system/system_registration.hpp>
#include <app/core/system/base_system.hpp>
#include <ui/core/base_ui_tool.hpp>
#include <ui/core/layout_descriptor.hpp>
#include <vector>

namespace VTX::UI::Core
{
	class ToolHandler : public App::Core::System::BaseSystem
	{
	  public:
		inline static const App::Application::System::SystemRegistration<ToolHandler> SYSTEM {};

	  public:
		ToolHandler();
		~ToolHandler();

		BaseVTXUITool * create( const ToolIdentifier & p_identifier );
		void			link( const ToolIdentifier & p_identifier, BaseVTXUITool * p_toolPtr );
		BaseVTXUITool & get( const ToolIdentifier & p_identifier ) const;
		bool			exists( const ToolIdentifier & p_identifier ) const;

	  private:
		std::map<ToolIdentifier, BaseVTXUITool *>	_mapTools;
		std::vector<std::unique_ptr<BaseVTXUITool>> _storedTools;
	};

	ToolHandler & TOOL_HANDLER();
} // namespace VTX::UI::Core

#endif