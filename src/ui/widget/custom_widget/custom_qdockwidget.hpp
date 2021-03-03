#ifndef __VTX_UI_WIDGET_CUSTOM_QDOCK_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_QDOCK_WIDGET__

#include "ui/qt_form/ui_custom_qdock_widget.h"
#include "ui/widget/custom_widget/settings_window.hpp"
#include <QDockWidget>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			class CustomQDockWidget : public QDockWidget
			{
				Q_OBJECT

			  public:
				CustomQDockWidget( QWidget * p_parent );
				~CustomQDockWidget() {};
				void setupModuleWindow();

				virtual QString				moduleIcon()			 = 0;
				virtual QString				moduleTitle()			 = 0;
				virtual std::vector<bool> * moduleWindowProperties() = 0;
				virtual QWidget *			moduleWidget()			 = 0;
				virtual QSettings *			saveSettings()			 = 0;

			  private:
				void setWindowContent( QWidget * moduleWidget );
				void setWindowIcon( QString winIcon );
				void setWindowTitle( QString winTitle );
				void setWindowProperties( std::vector<bool> * windowProperties );
				void setWindowDockableButton();
				void setWindowExitButton();

			  public slots:
				void onUndockButtonClicked();
				void onExitButtonClicked();
				void onLockButtonClicked();
				void onCitationsButtonClicked();
				// settings slots
				void onSettingsButtonClicked();
				void saveSettingsClicked();
				void loadSettingsClicked();
				void removeSettingsClicked();

			  protected:
				SettingsWindowWidget * settingsWindow = nullptr;
				Ui::CustomDockWidget   ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
