#include "settings_window.hpp"

SettingsWindowWidget::SettingsWindowWidget( QWidget * p_parent, QString extensionName ) : QDialog( p_parent )
{
	ui.setupUi( this );
}

void SettingsWindowWidget::populateSettingsList( QStringList * settingsNameList )
{
	for ( int settingName = 0; settingName < settingsNameList->size(); settingName++ )
	{
		ui.listWidgetSavedSettings->addItem( settingsNameList->at( settingName ) );
	}
}

void SettingsWindowWidget::onSaveSettingsClicked()
{
	QString settingName = ui.lineEditSettingsName->text();
	if ( settingName != "" )
	{
		ui.listWidgetSavedSettings->addItem( settingName );
	}
}
