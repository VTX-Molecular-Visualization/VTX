#ifndef __VTX_UI_WIDGET_SETTINGS_WINDOW_
#define __VTX_UI_WIDGET_SETTINGS_WINDOW_

#include "ui/qt_form/ui_settings_dialog.h"
#include <QDialog>
#include <QWidget>

class SettingsWindowWidget : public QDialog
{
	Q_OBJECT

  public:
	SettingsWindowWidget( QWidget * p_parent );
	~SettingsWindowWidget() {};
	void			   populateSettingsList( QStringList * settingsNameList );
	Ui::DialogSettings ui;

  public slots:
	void onSaveSettingsClicked();
};

#endif
