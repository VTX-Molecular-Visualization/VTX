#ifndef __VTX_UI_QT_TOOL_CHECK_UPDATE__
#define __VTX_UI_QT_TOOL_CHECK_UPDATE__

#include "core/tool_registration.hpp"
#include "keys.hpp"
#include "qt/base_qt_tool.hpp"
#include "qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QMenu>
#include <QObject>
#include <old/event/base_event_receiver_vtx.hpp>
#include <old/object3d/scene.hpp>

namespace VTX::UI::QT::Tool
{
	class CheckUpdateTool : public BaseQtTool, public VTX::Event::BaseEventReceiverVTX
	{
		REGISTER_TOOL( CheckUpdateTool, CHECK_UPDATE_KEY );

	  public:
		CheckUpdateTool();
		void instantiateTool() override;

		virtual void receiveEvent( const VTX::Event::VTXEvent & p_event );
	};
} // namespace VTX::UI::QT::Tool

#endif
