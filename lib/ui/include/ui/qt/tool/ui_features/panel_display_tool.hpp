#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_PANEL_DISPLAY__
#define __VTX_UI_QT_TOOL_UI_FEATURES_PANEL_DISPLAY__

#include "ui/core/tool_registration.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QAction>
#include <QMenu>
#include <app/old_app/event/base_event_receiver_vtx.hpp>

namespace VTX::UI::QT::Tool
{
	class PanelDisplayTool : public BaseQtTool, public VTX::Event::BaseEventReceiverVTX
	{
		REGISTER_TOOL( PanelDisplayTool, UI_FEATURE_PANEL_DISPLAY );

	  public:
		PanelDisplayTool();
		void instantiateTool() override;

		virtual void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

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
