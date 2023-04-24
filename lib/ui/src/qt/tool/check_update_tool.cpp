#include "ui/qt/tool/check_update_tool.hpp"
#include "ui/qt/action/main.hpp"
#include <app/old_app/setting.hpp>

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
				VTX_ACTION<QT::Action::Main::CheckForUpdate>();
			}
		}
	}
} // namespace VTX::UI::QT::Tool
