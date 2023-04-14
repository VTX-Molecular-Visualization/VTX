#ifndef __VTX_UI_WIDGET_MENU_EXTENSIONS_JOBS__
#define __VTX_UI_WIDGET_MENU_EXTENSIONS_JOBS__

#include "old_tool/ui/widget/main_menu/extensions/extension_cloud_connector_gui.hpp"
#include "old_tool/ui/widget/main_menu/extensions/extension_job_manager_gui.hpp"
#include "old_ui/ui/widget/base_manual_widget.hpp"
#include "old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QDockWidget>
#include <QWidget>

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
					class MenuExtensionsJobsWidgets : public MenuToolBlockWidget
					{
						VTX_WIDGET

					  public:
						~MenuExtensionsJobsWidgets();
						void localize() override;

					  protected:
						MenuExtensionsJobsWidgets( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						// Widgets list
						MenuToolButtonWidget * _jobManager	   = nullptr;
						MenuToolButtonWidget * _cloudConnector = nullptr;

						// Widgets windows
						ExtensionJobManagerGUI *	 _jobManagerWindow	   = nullptr;
						ExtensionCloudConnectorGUI * _cloudConnectorWindow = nullptr;

						// ACTIONS //
						void _showJobManagerWindowAction();
						void _showCloudConnectorWindowAction();
					};
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
