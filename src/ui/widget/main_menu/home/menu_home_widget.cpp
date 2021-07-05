#include "menu_home_widget.hpp"

namespace VTX::UI::Widget::MainMenu::Home
{
	MenuHomeWidget::MenuHomeWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	MenuHomeWidget::~MenuHomeWidget() {}

	void MenuHomeWidget::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		_session  = addToolBlock<MenuHomeSessionWidget>( "sessionBlockWidget" );
		_molecule = addToolBlock<MenuHomeMoleculeWidget>( "moleculeBlockWidget" );
		_windows  = addToolBlock<MenuHomeWindowsWidget>( "windowsBlockWidget" );
	}
	void MenuHomeWidget::_setupSlots() {}
	void MenuHomeWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Home
