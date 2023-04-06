#ifndef __VTX_TOOL_ANALYSIS_RMSD_UI__
#define __VTX_TOOL_ANALYSIS_RMSD_UI__

#include "analysis/rmsd/tool_info.hpp"
#include <old/event/base_event_receiver_vtx.hpp>
#include <ui/core/tool_registration.hpp>
#include <ui/qt/base_qt_tool.hpp>
#include <ui/qt/widget/main_menu/menu_toolbutton_widget.hpp>

namespace VTX::Tool::Analysis::RMSD::UI::QT
{
	class RMSDTool : public VTX::UI::QT::BaseQtTool, public VTX::Event::BaseEventReceiverVTX
	{
		REGISTER_TOOL( RMSDTool, TOOL_KEY );

	  public:
		RMSDTool();

		void instantiateTool() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  private:
		void _addButtonsInMainMenu();

		void _refreshButton() const;
		bool _checkRMSDEnableSate() const;
		void _computeRMSDAction() const;

	  private:
		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * _rmsdButton = nullptr;
	};
} // namespace VTX::Tool::Analysis::RMSD::UI::QT

#endif
