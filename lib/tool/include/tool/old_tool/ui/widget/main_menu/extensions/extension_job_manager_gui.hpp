#ifndef __VTX_UI_WIDGET_JOBMANAGER__
#define __VTX_UI_WIDGET_JOBMANAGER__

#include "tool/old_tool/ui/widget/custom_widget/custom_qdockwidget.hpp"
#include "tool/qt/qt_form/ui_job_manager_window.h"

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
					class ExtensionJobManagerGUI : public CustomQDockWidget
					{
						Q_OBJECT

					  public:
						explicit ExtensionJobManagerGUI( QWidget * parent );

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
						Ui::JobManagerGUI ui;
					};
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX

#endif