#include "ui/qt/tool/check_update_tool.hpp"
#include "ui/qt/action/main.hpp"
#include <app/event/global.hpp>
#include <app/old_app/setting.hpp>

namespace VTX::UI::QT::Tool
{
	CheckUpdateTool::CheckUpdateTool() : BaseQtTool() {}

	void CheckUpdateTool::instantiateTool() { _registerEvent( VTX::App::Event::Global::UI_APPLICATION_INITIALIZED ); }

	void CheckUpdateTool::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Event::Global::UI_APPLICATION_INITIALIZED )
		{
			if ( VTX_SETTING().getCheckVTXUpdateAtLaunch() )
			{
				VTX_ACTION<QT::Action::Main::CheckForUpdate>();
			}
		}
	}
} // namespace VTX::UI::QT::Tool
