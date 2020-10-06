#include "menu_tooltab_widget.hpp"
#include <QLabel>
#include <QPixmap>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuTooltabWidget::MenuTooltabWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
			MenuTooltabWidget::~MenuTooltabWidget() {}

			void MenuTooltabWidget::_addSeparator()
			{
				QPixmap * separatorImg = new QPixmap( ":/sprite/main_menu_separator.png" );
				QLabel *  separator	   = new QLabel( this );
				separator->setPixmap( *separatorImg );

				_horizontalLayout->insertWidget( _horizontalLayout->count() - 1, separator );
			}

			void MenuTooltabWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );
				setContentsMargins( 2, 2, 2, 2 );

				_horizontalLayout = new QHBoxLayout( this );
				_horizontalLayout->setObjectName( "horizontalLayout" );
				_horizontalLayout->setContentsMargins( 0, 0, 0, 0 );

				_horizontalSpacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
				_horizontalLayout->addItem( _horizontalSpacer );
			}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
