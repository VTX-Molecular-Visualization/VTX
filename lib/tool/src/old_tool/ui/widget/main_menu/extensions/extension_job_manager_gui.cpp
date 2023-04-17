#include "old_tool/ui/widget/main_menu/extensions/extension_job_manager_gui.hpp"

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
					ExtensionJobManagerGUI::ExtensionJobManagerGUI( QWidget * parent ) :
						CustomQDockWidget(
							parent->parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget() )
					{
						ui.setupUi( this );
						// set the mandatory modules' components (contnent, icon, title, properties, dock/exit buttons)
						setupModuleWindow();

						// set buttons icons and hovering style
						ui.pushButtonResume->setStyleSheet(
							"QPushButton {"
							"border-image: url(:/sprite/resume_icon.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"width: 15px;"
							"height : 15px;"
							"}"
							"QPushButton:hover {"
							"border-image: url(:/sprite/resume_icon_hovered.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"}" );
						ui.pushButtonPause->setStyleSheet(
							"QPushButton {"
							"border-image: url(:/sprite/pause_icon.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"width: 15px;"
							"height : 15px;"
							"}"
							"QPushButton:hover {"
							"border-image: url(:/sprite/pause_icon_hovered.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"}" );
						ui.pushButtonStop->setStyleSheet(
							"QPushButton {"
							"border-image: url(:/sprite/stop_icon.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"width: 15px;"
							"height : 15px;"
							"}"
							"QPushButton:hover {"
							"border-image: url(:/sprite/stop_icon_hovered.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"}" );
						ui.pushButtonRemove->setStyleSheet(
							"QPushButton {"
							"border-image: url(:/sprite/remove_icon.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"width: 15px;"
							"height : 15px;"
							"}"
							"QPushButton:hover {"
							"border-image: url(:/sprite/remove_icon_hovered.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"}" );
						ui.pushButtonRetrieveJobs->setStyleSheet(
							"QPushButton {"
							"border-image: url(:/sprite/refresh_icon.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"width: 15px;"
							"height : 15px;"
							"}"
							"QPushButton:hover {"
							"border-image: url(:/sprite/refresh_icon_hovered.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"}" );

						// retrieve jobs list
					}

					QString ExtensionJobManagerGUI::moduleIcon()
					{
						// set module window icon
						return QString( ":/sprite/job_manager.png" );
					}

					QString ExtensionJobManagerGUI::moduleTitle()
					{
						// set module window title
						return QString( "Job Manager" );
					}

					std::vector<bool> * ExtensionJobManagerGUI::moduleWindowProperties()
					{
						// set window properties:
						// 0: savable
						// 1: citable
						std::vector<bool> * windowProperties = new std::vector<bool>();
						windowProperties->push_back( false );
						windowProperties->push_back( false );
						return windowProperties;
					}

					QWidget * ExtensionJobManagerGUI::moduleWidget()
					{
						// set module window widgets with a widget containing all the module's widgets
						return ui.widgetContainer;
					}

					QString ExtensionJobManagerGUI::moduleCitations() { return QString( "" ); }

					void ExtensionJobManagerGUI::saveSettings( QSettings * settings ) {}

					void ExtensionJobManagerGUI::loadSettings( QSettings * settings ) {}

				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
