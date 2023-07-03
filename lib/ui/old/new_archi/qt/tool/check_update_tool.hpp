#ifndef __VTX_UI_QT_TOOL_CHECK_UPDATE__
#define __VTX_UI_QT_TOOL_CHECK_UPDATE__

#include "ui/core/tool_registration.hpp"
#include "keys.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include "ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QMenu>
#include <QObject>
#include <app/old/core/event/base_event_receiver_vtx.hpp>
#include <app/old/application/scene.hpp>

namespace VTX::UI::QT::Tool
{
	class CheckUpdateTool : public BaseQtTool, public VTX::App::Old::Core::Event::BaseEventReceiverVTX
	{
		REGISTER_TOOL( CheckUpdateTool, CHECK_UPDATE_KEY );

	  public:
		CheckUpdateTool();
		void instantiateTool() override;

		virtual void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event );
	};
} // namespace VTX::UI::QT::Tool

#endif
