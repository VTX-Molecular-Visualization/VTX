#include "custom_qdockwidget.hpp"

CustomQDockWidget::CustomQDockWidget( QWidget * p_parent )
{
	if ( settingsWindow == nullptr )
	{
		settingsWindow = new SettingsWindowWidget( this );
	}
}

void CustomQDockWidget::set_window_properties( bool savable, bool lockable )
{
	if ( savable )
	{
		// set up save/load settings button
		// QWidget *	  title_bar = this->titleBarWidget();
		// QHBoxLayout * layout	= new QHBoxLayout();
		// title_bar->setLayout( layout );
		// QPushButton * settingButton = new QPushButton();
		// QIcon		  infoIcon		= QIcon( ":/sprite/settings_icon.png" );
		// settingButton->setIcon( infoIcon );
		// settingButton->setStyleSheet( "QPushButton { background-color: rgba(255, 255, 255, 0); }" );
		// layout->addWidget( settingButton );
		// this->setTitleBarWidget( title_bar );
		// QObject::connect( settingButton, SIGNAL( clicked() ), this, SLOT( onSettingsButtonClicked() ) );
	}
	if ( lockable ) {}
}

void CustomQDockWidget::save_settings() {}

void CustomQDockWidget::load_settings() {}

void CustomQDockWidget::onSettingsButtonClicked()
{
	if ( !settingsWindow->isVisible() )
	{
		settingsWindow->show();
	}
	else
	{
		settingsWindow->raise();
	}
}
