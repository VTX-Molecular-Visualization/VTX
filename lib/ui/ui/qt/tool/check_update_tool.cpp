#include "check_update_tool.hpp"
#include "qt/action/main.hpp"
#include <old/action/action_manager.hpp>
#include <old/setting.hpp>

namespace VTX::UI::QT::Tool
{
	CheckUpdateTool::CheckUpdateTool() : BaseQtTool() {}

	void CheckUpdateTool::instantiateTool() { _registerEvent( VTX::Event::Global::UI_APPLICATION_INITIALIZED ); }

	void CheckUpdateTool::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::UI_APPLICATION_INITIALIZED )
		{
			if ( VTX_SETTING().getCheckVTXUpdateAtLaunch() )
			{
				VTX_ACTION( new QT::Action::Main::CheckForUpdate() );
			}
		}
	}
} // namespace VTX::UI::QT::Tool
