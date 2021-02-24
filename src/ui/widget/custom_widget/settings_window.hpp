#ifndef __VTX_UI_WIDGET_SETTINGS_Window_
#define __VTX_UI_WIDGET_SETTINGS_Window_

#include "ui/qt_form/ui_settings_dialog.h"
#include <QDialog>
#include <QSettings>
#include <QWidget>

class SettingsWindowWidget : public QDialog
{
	Q_OBJECT

  public:
	SettingsWindowWidget( QWidget * p_parent, QString extensionName );
	~SettingsWindowWidget() {};

	void populateSettingsList( QStringList * settingsNameList );

	Ui::DialogSettings ui;

  public slots:
	void onSaveSettingsClicked();

  private:
};

#endif
