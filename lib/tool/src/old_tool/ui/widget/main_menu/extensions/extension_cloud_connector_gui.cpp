#include "old_tool/ui/widget/main_menu/extensions/extension_cloud_connector_gui.hpp"

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
					ExtensionCloudConnectorGUI::ExtensionCloudConnectorGUI( QWidget * parent ) :
						CustomQDockWidget(
							parent->parentWidget()->parentWidget()->parentWidget()->parentWidget()->parentWidget() )
					{
						ui.setupUi( this );
						// set the mandatory modules' components (contnent, icon, title, properties, dock/exit buttons)
						setupModuleWindow();

						// set buttons icons and hovering style
						ui.pushButtonConnectCloud->setStyleSheet(
							"QPushButton {"
							"border-image: url(:/sprite/add_icon.png);"
							"background-color: rgba(255, 255, 255, 0);"
							"background-repeat: no-repeat;"
							"width: 15px;"
							"height : 15px;"
							"}"
							"QPushButton:hover {"
							"border-image: url(:/sprite/add_icon_hovered.png);"
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
					}

					QString ExtensionCloudConnectorGUI::moduleIcon()
					{
						// set module window icon
						return QString( ":/sprite/cloud_connector.png" );
					}

					QString ExtensionCloudConnectorGUI::moduleTitle()
					{
						// set module window title
						return QString( "Cloud Connector" );
					}

					std::vector<bool> * ExtensionCloudConnectorGUI::moduleWindowProperties()
					{
						// set window properties:
						// 0: savable
						// 1: citable
						std::vector<bool> * windowProperties = new std::vector<bool>();
						windowProperties->push_back( false );
						windowProperties->push_back( false );
						return windowProperties;
					}

					QWidget * ExtensionCloudConnectorGUI::moduleWidget()
					{
						// set module window widgets with a widget containing all the module's widgets
						return ui.widgetContainer;
					}

					QString ExtensionCloudConnectorGUI::moduleCitations() { return QString( "" ); }

					void ExtensionCloudConnectorGUI::saveSettings( QSettings * settings ) {}

					void ExtensionCloudConnectorGUI::loadSettings( QSettings * settings ) {}

				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
