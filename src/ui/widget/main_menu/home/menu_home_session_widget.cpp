#include "menu_home_session_widget.hpp"
#include "action/main.hpp"
#include "define.hpp"
#include "setting.hpp"
#include "ui/widget/custom_widget/indexed_action.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QFileDialog>
#include <vector>

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
					MenuHomeSessionWidget::~MenuHomeSessionWidget() {}

					void MenuHomeSessionWidget::_openFile()
					{
						// TODO : Filter file type
						const QString filename = QFileDialog::getOpenFileName( this, "Open Molecule", "", "*" );
						Path *		  path	   = new Path( filename.toStdString() );

						VTX_ACTION( new Action::Main::Open( path ), true );
					}

					void MenuHomeSessionWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						setTitle( "Session" );

						_newSessionButton = WidgetFactory::get().getWidget<MenuToolButtonWidget>( this, "newSessionButton" );
						_newSessionButton->setData( "New", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
						pushButton( *_newSessionButton, 0 );

						_openSessionButton = WidgetFactory::get().getWidget<MenuToolButtonWidget>( this, "openSessionButton" );
						_openSessionButton->setData( "Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_openSessionButton, 1 );

						_openRecentSessionButton = WidgetFactory::get().getWidget<MenuToolButtonWidget>( this, "openRecentSessionButton" );
						_openRecentSessionButton->setData( "Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_openRecentSessionButton, 1 );

						_saveSessionButton = WidgetFactory::get().getWidget<MenuToolButtonWidget>( this, "saveSessionButton" );
						_saveSessionButton->setData( "Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_saveSessionButton, 2 );

						_saveAsSessionButton = WidgetFactory::get().getWidget<MenuToolButtonWidget>( this, "saveAsSessionButton" );
						_saveAsSessionButton->setData( "Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_saveAsSessionButton, 2 );

						QMenu * const recentSessionMenu = _generateRecentSessionsMenu();
						_openRecentSessionButton->setMenu( recentSessionMenu );

						validate();
					}
					void MenuHomeSessionWidget::_setupSlots() { _openSessionButton->setTriggerAction( this, &MenuHomeSessionWidget::_openFile ); }
					void MenuHomeSessionWidget::localize() {}

					QMenu * const MenuHomeSessionWidget::_generateRecentSessionsMenu() const
					{
						QMenu * const recentSessionMenu = new QMenu( _openRecentSessionButton );
						int			  actionIndex		= 0;

						const std::vector<VTX::Path> recentFiles = Setting::recentLoadingPath;

						for ( auto it : recentFiles )
						{
							CustomWidget::IndexedAction * const action = new CustomWidget::IndexedAction( actionIndex, recentSessionMenu );
							const QString						path   = QString::fromStdString( it.string() );
							action->setText( path );

							connect( action, &CustomWidget::IndexedAction::triggeredWithIndex, this, &MenuHomeSessionWidget::_loadRecentSession );

							recentSessionMenu->addAction( action );
							actionIndex++;
						}

						return recentSessionMenu;
					}

					void MenuHomeSessionWidget::_loadRecentSession( const int & p_ptrSessionIndex ) const
					{
						VTX_INFO( "Load " + Setting::recentLoadingPath[ p_ptrSessionIndex ].string() );
					}

				} // namespace Home
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
