#include "menu_measurement_widget.hpp"

namespace VTX::UI::Widget::MainMenu::Measurement
{
	MenuMeasurementWidget::MenuMeasurementWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	MenuMeasurementWidget::~MenuMeasurementWidget() {}

	void MenuMeasurementWidget::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		_distance = addToolBlock<MenuMeasurementDistanceWidget>( "distanceBlockWidget" );
	}
	void MenuMeasurementWidget::_setupSlots() {}
	void MenuMeasurementWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Measurement
