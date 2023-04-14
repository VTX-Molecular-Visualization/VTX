#ifndef __VTX_UI_WIDGET_CLOUDMANAGER__
#define __VTX_UI_WIDGET_CLOUDMANAGER__

#include "old_ui/ui/qt_form/ui_cloud_connector_window.h"
#include "old_ui/ui/widget/custom_widget/custom_qdockwidget.hpp"

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
					class ExtensionCloudConnectorGUI : public CustomQDockWidget
					{
						Q_OBJECT

					  public:
						explicit ExtensionCloudConnectorGUI( QWidget * parent );

						QString				moduleIcon() override;
						QString				moduleTitle() override;
						std::vector<bool> * moduleWindowProperties() override;
						QWidget *			moduleWidget() override;
						QString				moduleCitations() override;
						void				saveSettings( QSettings * savedSettings ) override;
						void				loadSettings( QSettings * settings ) override;

					  private:
					  private slots:

					  private:
						Ui::ManageCloudsWindowDialog ui;
					};
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX

#endif
