#include "menu_main_session_widget.hpp"
#include "action/main.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QFileDialog>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuMainSessionWidget::~MenuMainSessionWidget() {}

			void MenuMainSessionWidget::_openFile()
			{
				// TODO : Filter file type
				const QString filename = QFileDialog::getOpenFileName( this, "Open Molecule", "", "*" );
				Path *		  path	   = new Path( filename.toStdString() );

				VTX_ACTION( new Action::Main::Open( path ), true );
			}

			void MenuMainSessionWidget::_setupUi( const QString & p_name )
			{
				MenuToolBlockWidget::_setupUi( p_name );

				setTitle( "Session" );

				_newSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "newSessionButton" );
				_newSessionButton->setData( "New", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
				pushButton( *_newSessionButton, 0 );

				_openSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "openSessionButton" );
				_openSessionButton->setData( "Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_openSessionButton, 1 );

				_openRecentSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "openRecentSessionButton" );
				_openRecentSessionButton->setData( "Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_openRecentSessionButton, 1 );

				_saveSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "saveSessionButton" );
				_saveSessionButton->setData( "Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_saveSessionButton, 2 );

				_saveAsSessionButton = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "saveAsSessionButton" );
				_saveAsSessionButton->setData( "Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_saveAsSessionButton, 2 );

				validate();
			}
			void MenuMainSessionWidget::_setupSlots() { _openSessionButton->setTriggerAction( this, &MenuMainSessionWidget::_openFile ); }
			void MenuMainSessionWidget::localize() {}
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
