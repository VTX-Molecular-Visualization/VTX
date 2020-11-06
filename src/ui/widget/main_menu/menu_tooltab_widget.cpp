#include "menu_tooltab_widget.hpp"
#include <QLabel>
#include <QPixmap>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				MenuTooltabWidget::MenuTooltabWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
				MenuTooltabWidget::~MenuTooltabWidget() {}

				void MenuTooltabWidget::_addSeparator()
				{
					QLabel *  separator	   = new QLabel( this );
					separator->setPixmap( QPixmap( ":/sprite/main_menu_separator.png" ) );

					_horizontalLayout->insertWidget( _horizontalLayout->count() - 1, separator );
				}

				void MenuTooltabWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );
					setContentsMargins( 2, 2, 2, 2 );

					_horizontalLayout = new QHBoxLayout( this );
					_horizontalLayout->setContentsMargins( 0, 0, 0, 0 );
					_horizontalLayout->setSpacing( 2 );

					_horizontalLayout->addStretch( 1000 );
				}
			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
