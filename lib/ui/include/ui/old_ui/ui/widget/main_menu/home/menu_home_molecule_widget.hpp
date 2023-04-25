#ifndef __VTX_UI_WIDGET_MENU_HOME_MOLECULE__
#define __VTX_UI_WIDGET_MENU_HOME_MOLECULE__

#include <app/core/event/vtx_event.hpp>
#include "ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_submenu_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Home
{
	class MenuHomeMoleculeWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

		void receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;

	  protected:
		MenuHomeMoleculeWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _enableButtons( const bool p_enable );

	  private:
		// Export
		MenuToolButtonWidget * _openTrajectoryButton  = nullptr;
		MenuToolButtonWidget * _exportSelectionButton = nullptr;

		void _exportSelection() const;
		void _openTrajectory() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Home
#endif
