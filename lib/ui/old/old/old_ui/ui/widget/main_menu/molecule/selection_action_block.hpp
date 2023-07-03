#ifndef __VTX_UI_WIDGET_MENU_MOLECULE_SELECTION_ACTION_BLOCK__
#define __VTX_UI_WIDGET_MENU_MOLECULE_SELECTION_ACTION_BLOCK__

#include "ui/old_ui/ui/widget/custom_widget/trajectory_frames_menu.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_submenu_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>
#include <app/old/core/event/vtx_event.hpp>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	class SelectionActionBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

		void receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) override;

	  protected:
		SelectionActionBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _enableButtons( const bool p_enable );

	  private:
		// Selection actions 1
		MenuToolButtonWidget *		  _extract = nullptr;
		MenuToolButtonSubmenuWidget * _copy	   = nullptr;
		MenuToolButtonWidget *		  _delete  = nullptr;

		// Visibility actions
		MenuToolButtonWidget * _show = nullptr;
		MenuToolButtonWidget * _hide = nullptr;
		MenuToolButtonWidget * _solo = nullptr;

		// Export
		MenuToolButtonWidget * _exportSelectionButton = nullptr;

		// Copy submenu
		CustomWidget::TrajectoryFramesMenu * _copyFrameSubmenu = nullptr;

		void _copySelection() const;
		void _copyFrameSelection( const int p_frame ) const;
		void _extractSelection() const;
		void _deleteSelection() const;

		void _showSelection() const;
		void _hideSelection() const;
		void _soloSelection() const;

		void _exportSelection() const;

		void _updateFrameSubmenu() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Molecule
#endif
