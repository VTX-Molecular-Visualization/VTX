#ifndef __VTX_UI_WIDGET_SETTINGS_Window_
#define __VTX_UI_WIDGET_SETTINGS_Window_

#include "ui/qt_form/settings_dialog.h"
#include <QDialog>
#include <QWidget>

class SettingsWindowWidget : public QDialog
{
	Q_OBJECT

  public:
	SettingsWindowWidget( QWidget * p_parent );
	~SettingsWindowWidget() {};

  public slots:
	void onSaveSettingsClicked();
	void onLoadSettingsClicked();
	void onRemoveSettingsClicked();

  private:
	Ui::DialogSettings ui;
};

#endif
