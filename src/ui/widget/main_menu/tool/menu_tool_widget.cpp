#include "menu_tool_widget.hpp"

namespace VTX::UI::Widget::MainMenu::Tool
{
	MenuToolWidget::MenuToolWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	MenuToolWidget::~MenuToolWidget() {}

	void MenuToolWidget::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		_measurement = addToolBlock<MenuToolMeasurementWidget>( "measurementBlockWidget" );
		_structuralAlignment = addToolBlock<MenuToolStructuralAlignmentWidget>( "structuralAlignmentBlockWidget" );
		
	}
	void MenuToolWidget::_setupSlots() {}
	void MenuToolWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Measurement
