#ifndef __VTX_UI_WIDGET_MENU_MEASUREMENT__
#define __VTX_UI_WIDGET_MENU_MEASUREMENT__

#include "menu_measurement_distance_widget.hpp"
#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Measurement
{
	class MenuMeasurementWidget : public MenuTooltabWidget
	{
		VTX_WIDGET

	  public:
		~MenuMeasurementWidget();
		void localize() override;

	  protected:
		MenuMeasurementWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		MenuMeasurementDistanceWidget * _distance = nullptr;
	};
} // namespace VTX::UI::Widget::MainMenu::Measurement
#endif
