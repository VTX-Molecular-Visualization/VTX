#include "menu_home_widget.hpp"
#include "menu_home_molecule_widget.hpp"
#include "menu_home_session_widget.hpp"
#include "menu_home_windows_widget.hpp"

namespace VTX::UI::Widget::MainMenu::Home
{
	MenuHomeWidget::MenuHomeWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	MenuHomeWidget::~MenuHomeWidget() {}

	void MenuHomeWidget::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		addToolBlock<MenuHomeSessionWidget>( "sessionBlockWidget" );
		addToolBlock<MenuHomeMoleculeWidget>( "moleculeWidget" );
		addToolBlock<MenuHomeWindowsWidget>( "windowsBlockWidget" );
	}
	void MenuHomeWidget::_setupSlots() {}
	void MenuHomeWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Home
