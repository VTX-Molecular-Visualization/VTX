#include "menu_main_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuMainWidget::MenuMainWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
			MenuMainWidget::~MenuMainWidget() {}

			void MenuMainWidget::_setupUi( const QString & p_name )
			{
				MenuTooltabWidget::_setupUi( p_name );

				_session  = addToolBlock<MenuMainSessionWidget>( "sessionBlockWidget" );
				_molecule = addToolBlock<MenuMainMoleculeWidget>( "moleculeBlockWidget" );
			}
			void MenuMainWidget::_setupSlots() {}
			void MenuMainWidget::localize() {}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
