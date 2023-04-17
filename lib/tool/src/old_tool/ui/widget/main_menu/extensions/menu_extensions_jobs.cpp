#include "tool/old_tool/ui/widget/main_menu/extensions/menu_extensions_jobs.hpp"
#include <app/old_app/action/main.hpp>
#include <app/old_app/io/filesystem.hpp>
#include <ui/old_ui/ui/widget_factory.hpp>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Extensions
				{
					MenuExtensionsJobsWidgets::~MenuExtensionsJobsWidgets() {}

					void MenuExtensionsJobsWidgets::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						// Add extensions buttons
						_cloudConnector = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "cloudConnectorButton" );
						_cloudConnector->setData(
							"Cloud\nConnector", ":/sprite/cloud_connector.png", Qt::Orientation::Vertical );
						pushButton( *_cloudConnector, 0 );

						_jobManager
							= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "jobManagerButton" );
						_jobManager->setData( "Job\nManager", ":/sprite/job_manager.png", Qt::Orientation::Vertical );
						pushButton( *_jobManager, 1 );

						validate();
					}
					void MenuExtensionsJobsWidgets::_setupSlots()
					{
						_cloudConnector->setTriggerAction(
							this, &MenuExtensionsJobsWidgets::_showCloudConnectorWindowAction );
						_jobManager->setTriggerAction( this, &MenuExtensionsJobsWidgets::_showJobManagerWindowAction );
					}
					void MenuExtensionsJobsWidgets::localize() { setTitle( "Jobs" ); }

					void MenuExtensionsJobsWidgets::_showCloudConnectorWindowAction()
					{
						if ( _cloudConnectorWindow == nullptr )
						{
							_cloudConnectorWindow = new ExtensionCloudConnectorGUI( this );
							_cloudConnectorWindow->show();
						}
						else
						{
							if ( !_cloudConnectorWindow->isVisible() )
							{
								_cloudConnectorWindow->show();
							}
							else
							{
								_cloudConnectorWindow->raise();
							}
						}
					}

					void MenuExtensionsJobsWidgets::_showJobManagerWindowAction()
					{
						if ( _jobManagerWindow == nullptr )
						{
							_jobManagerWindow = new ExtensionJobManagerGUI( this );
							_jobManagerWindow->show();
						}
						else
						{
							if ( !_jobManagerWindow->isVisible() )
							{
								_jobManagerWindow->show();
							}
							else
							{
								_jobManagerWindow->raise();
							}
						}
					}

				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX