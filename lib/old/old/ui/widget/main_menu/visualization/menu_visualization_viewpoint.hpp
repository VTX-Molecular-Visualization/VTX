#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_VIEWPOINT__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_VIEWPOINT__

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationViewpointWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		MenuVisualizationViewpointWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _enableDeleteButtonState( const bool p_enable );

	  private:
		MenuToolButtonWidget * _createViewpointButton = nullptr;
		MenuToolButtonWidget * _deleteViewpointButton = nullptr;

		void _createViewpointAction() const;
		void _deleteViewpointAction() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
