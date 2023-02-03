#ifndef __VTX_UI_DEFAULT_TOOL_RENDER_WINDOW__
#define __VTX_UI_DEFAULT_TOOL_RENDER_WINDOW__

#include "__new_archi/tool/analysis/rmsd/tool_info.hpp"
#include "__new_archi/ui/core/tool_registration.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include <QObject>

namespace VTX::Tool::Analysis::RMSD::UI::QT
{
	class RMSDTool : public VTX::UI::Core::BaseVTXUITool, public QObject, public VTX::Event::BaseEventReceiverVTX
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
