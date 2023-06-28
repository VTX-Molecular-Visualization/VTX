#include "ui/qt/tool/check_update_tool.hpp"
#include "ui/qt/action/main.hpp"
#include <app/old/event/global.hpp>
#include <app/old/application/setting.hpp>

namespace VTX::UI::QT::Tool
{
	CheckUpdateTool::CheckUpdateTool() : BaseQtTool() {}

	void CheckUpdateTool::instantiateTool() { _registerEvent( VTX::App::Old::Event::Global::UI_APPLICATION_INITIALIZED ); }

	void CheckUpdateTool::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::UI_APPLICATION_INITIALIZED )
		{
			if ( VTX_SETTING().getCheckVTXUpdateAtLaunch() )
			{
				VTX_ACTION<QT::Action::Main::CheckForUpdate>();
			}
		}
	}
} // namespace VTX::UI::QT::Tool
