#ifndef __VTX_UI_WIDGET_JOBMANAGER__
#define __VTX_UI_WIDGET_JOBMANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "extensions/job_manager/extension_job_manager.hpp"
#include "ui/qt_form/ui_job_manager_window.h"
#include "ui/widget/custom_widget/custom_qdockwidget.hpp"

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
