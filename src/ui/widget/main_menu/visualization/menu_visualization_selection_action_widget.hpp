#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_SELECTION_ACTION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_SELECTION_ACTION__

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationSelectionActionWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		MenuVisualizationSelectionActionWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _enableButtons( const bool p_enable );

	  private:
		// Selection actions 1
		MenuToolButtonWidget * _extract = nullptr;
		MenuToolButtonWidget * _copy	= nullptr;
		MenuToolButtonWidget * _delete	= nullptr;

		// Visibility actions
		MenuToolButtonWidget * _show = nullptr;
		MenuToolButtonWidget * _hide = nullptr;

		// Export
		MenuToolButtonWidget * _exportSelectionButton = nullptr;

		void _copySelection() const;
		void _extractSelection() const;
		void _deleteSelection() const;

		void _showSelection() const;
		void _hideSelection() const;

		void _exportSelection() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
