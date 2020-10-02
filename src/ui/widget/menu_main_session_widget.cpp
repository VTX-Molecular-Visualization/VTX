#include "menu_main_session_widget.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuMainSessionWidget::~MenuMainSessionWidget() {}
			void MenuMainSessionWidget::setupUi( const QString & p_name )
			{
				MenuToolBlockWidget::setupUi( p_name );

				setTitle( "Session" );

				_newSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "newSessionButton" );
				_newSessionButton->setData( "New", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
				addButton( *_newSessionButton, 0, 0, 2, 1 );

				_openSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "openSessionButton" );
				_openSessionButton->setData( "Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
				addButton( *_openSessionButton, 0, 1 );

				_openRecentSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "openRecentSessionButton" );
				_openRecentSessionButton->setData( "Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );
				addButton( *_openRecentSessionButton, 1, 1 );

				_saveSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "saveSessionButton" );
				_saveSessionButton->setData( "Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );
				addButton( *_saveSessionButton, 0, 2 );

				_saveAsSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "saveAsSessionButton" );
				_saveAsSessionButton->setData( "Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
				addButton( *_saveAsSessionButton, 1, 2 );

				validate();
			}
			void MenuMainSessionWidget::setupSlots() {}
			void MenuMainSessionWidget::localize() {}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
