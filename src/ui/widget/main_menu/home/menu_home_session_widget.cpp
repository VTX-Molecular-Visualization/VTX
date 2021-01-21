#include "menu_home_session_widget.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
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
						FilePath *	  path	   = new FilePath( filename.toStdString() );

						VTX_ACTION( new Action::Main::Open( path ), true );
					}

					void MenuHomeSessionWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						setTitle( "Session" );

						_newSessionButton
							= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "newSessionButton" );
						_newSessionButton->setData( "New", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
						pushButton( *_newSessionButton, 0 );

						_openSessionButton
							= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "openSessionButton" );
						_openSessionButton->setData(
							"Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_openSessionButton, 1 );

						_openRecentSessionButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "openRecentSessionButton" );
						_openRecentSessionButton->setData(
							"Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_openRecentSessionButton, 1 );

						_saveSessionButton
							= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "saveSessionButton" );
						_saveSessionButton->setData(
							"Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_saveSessionButton, 2 );

						_saveAsSessionButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "saveAsSessionButton" );
						_saveAsSessionButton->setData(
							"Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_saveAsSessionButton, 2 );

						_loadSettingsButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "loadSettingsButton" );
						_loadSettingsButton->setData(
							"Load settings", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_loadSettingsButton, 3 );

						_saveSettingsButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "saveSettingsButton" );
						_saveSettingsButton->setData(
							"Save settings", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_saveSettingsButton, 3 );

						QMenu * const recentSessionMenu = _generateRecentSessionsMenu();
						_openRecentSessionButton->setMenu( recentSessionMenu );

						validate();
					}
					void MenuHomeSessionWidget::_setupSlots()
					{
						_openSessionButton->setTriggerAction( this, &MenuHomeSessionWidget::_openFile );
						_loadSettingsButton->setTriggerAction( this, &MenuHomeSessionWidget::_loadSettings );
						_saveSettingsButton->setTriggerAction( this, &MenuHomeSessionWidget::_saveSettings );
					}
					void MenuHomeSessionWidget::localize() {}

					QMenu * const MenuHomeSessionWidget::_generateRecentSessionsMenu() const
					{
						QMenu * const recentSessionMenu = new QMenu( _openRecentSessionButton );
						int			  actionIndex		= 0;

						const std::vector<VTX::FilePath> & recentFiles = Setting::recentLoadingPath;

						for ( const VTX::FilePath & recentFile : recentFiles )
						{
							CustomWidget::IndexedAction * const action
								= new CustomWidget::IndexedAction( actionIndex, recentSessionMenu );
							const QString path = QString::fromStdString( recentFile.string() );
							action->setText( path );

							connect( action,
									 &CustomWidget::IndexedAction::triggeredWithIndex,
									 this,
									 &MenuHomeSessionWidget::_loadRecentSession );

							recentSessionMenu->addAction( action );
							actionIndex++;
						}

						return recentSessionMenu;
					}

					void MenuHomeSessionWidget::_loadRecentSession( const int & p_ptrSessionIndex ) const
					{
						VTX_INFO( "Load " + Setting::recentLoadingPath[ p_ptrSessionIndex ].string() );
					}

					void MenuHomeSessionWidget::_loadSettings() const { VTX_ACTION( new Action::Setting::Load() ); }

					void MenuHomeSessionWidget::_saveSettings() const { VTX_ACTION( new Action::Setting::Save() ); }

				} // namespace Home
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
