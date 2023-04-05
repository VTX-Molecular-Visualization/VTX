#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_PANEL_DISPLAY__
#define __VTX_UI_QT_TOOL_UI_FEATURES_PANEL_DISPLAY__

#include "core/tool_registration.hpp"
#include "qt/base_qt_tool.hpp"
#include "qt/tool/keys.hpp"
#include "qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QAction>
#include <QMenu>
#include <old/event/base_event_receiver_vtx.hpp>

namespace VTX::UI::QT::Tool
{
	class PanelDisplayTool : public BaseQtTool, public Event::BaseEventReceiverVTX
	{
		REGISTER_TOOL( PanelDisplayTool, UI_FEATURE_PANEL_DISPLAY );

	  public:
		PanelDisplayTool();
		void instantiateTool() override;

		virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

	  private:
		QMenu *									 _panelListMenu	  = nullptr;
		Widget::MainMenu::MenuToolButtonWidget * _panelListButton = nullptr;

		void _addButtonsInMainMenu();

		void _populatePanelMenu();
		void _refreshPanelVisibilityState();

		void _togglePanelVisibility( const QAction * const p_action );
	};
} // namespace VTX::UI::QT::Tool

#endif
