#ifndef __VTX_UI_WIDGET_CUSTOM_QDOCK_WIDGET__
#define __VTX_UI_WIDGET_CUSTOM_QDOCK_WIDGET__

#include "old_ui/ui/qt_form/ui_custom_qdock_widget.h"
#include "old_ui/ui/widget/custom_widget/citations_window.hpp"
#include "old_ui/ui/widget/custom_widget/settings_window.hpp"
#include <QDockWidget>
#include <QFileDialog>
#include <QSettings>
#include <QStandardPaths>
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

				virtual QString				moduleIcon()							  = 0;
				virtual QString				moduleTitle()							  = 0;
				virtual std::vector<bool> * moduleWindowProperties()				  = 0;
				virtual QString				moduleCitations()						  = 0;
				virtual QWidget *			moduleWidget()							  = 0;
				virtual void				saveSettings( QSettings * savedSettings ) = 0;
				virtual void				loadSettings( QSettings * settings )	  = 0;

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
				void onCitationsButtonClicked();
				// settings slots
				void onSettingsButtonClicked();
				void saveSettingsClicked();
				void loadSettingsClicked();
				void removeSettingsClicked();

			  protected:
				void setupModuleWindow();

				SettingsWindowWidget *	settingsWindow	= nullptr;
				CitationsWindowWidget * citationsWindow = nullptr;
				Ui::CustomDockWidget	ui;
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
