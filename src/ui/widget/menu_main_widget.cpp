#include "menu_main_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuMainWidget::MenuMainWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
			MenuMainWidget::~MenuMainWidget() {}

			void MenuMainWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );
				setContentsMargins( 2, 2, 2, 2 );

				_horizontalLayout = new QHBoxLayout( this );
				_horizontalLayout->setObjectName( "horizontalLayout" );
				_horizontalLayout->setContentsMargins( 0, 0, 0, 0 );

				_session = WidgetFactory::get().GetWidget<MenuMainSessionWidget>( this, "sessionBlockWidget" );
				_horizontalLayout->addWidget( _session );

				_horizontalSpacer = new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
				_horizontalLayout->addItem( _horizontalSpacer );
			}
			void MenuMainWidget::_setupSlots() {}
			void MenuMainWidget::localize() {}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
