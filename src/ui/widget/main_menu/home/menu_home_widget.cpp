#include "menu_home_widget.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Home
				{
					MenuHomeWidget::MenuHomeWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
					MenuHomeWidget::~MenuHomeWidget() {}

					void MenuHomeWidget::_setupUi( const QString & p_name )
					{
						MenuTooltabWidget::_setupUi( p_name );

						_session  = addToolBlock<MenuHomeSessionWidget>( "sessionBlockWidget" );
						_molecule = addToolBlock<MenuHomeMoleculeWidget>( "moleculeBlockWidget" );
					}
					void MenuHomeWidget::_setupSlots() {}
					void MenuHomeWidget::localize() {}
				} // namespace Home
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
