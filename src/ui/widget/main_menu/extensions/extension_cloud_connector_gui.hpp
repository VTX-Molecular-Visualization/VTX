#ifndef __VTX_UI_WIDGET_CLOUDMANAGER__
#define __VTX_UI_WIDGET_CLOUDMANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/qt_form/ui_cloud_connector_window.h"
#include "ui/widget/custom_widget/custom_qdockwidget.hpp"

namespace VTX::UI::Widget::MainMenu::Extensions
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
		void onPushButtonConnectCloudClicked();
		void onPushButtonConnectRemoveClicked();

	  private:
		Ui::ManageCloudsWindowDialog ui;
	};
} // namespace VTX::UI::Widget::MainMenu::Extensions

#endif
