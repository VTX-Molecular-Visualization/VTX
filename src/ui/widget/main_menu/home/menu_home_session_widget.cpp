#include "menu_home_session_widget.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "define.hpp"
#include "setting.hpp"
#include "ui/dialog.hpp"
#include "ui/widget/custom_widget/indexed_action.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QFileDialog>
#include <vector>

namespace VTX::UI::Widget::MainMenu::Home
{
	MenuHomeSessionWidget::MenuHomeSessionWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::RECENT_FILES_CHANGE );
	}

	MenuHomeSessionWidget::~MenuHomeSessionWidget() {}

	void MenuHomeSessionWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::RECENT_FILES_CHANGE )
			_refreshRecentFiles();
	}

	void MenuHomeSessionWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		setTitle( "Session" );

		_newSessionButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "newSessionButton" );
		_newSessionButton->setData( "New", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
		pushButton( *_newSessionButton, 0 );

		_openSessionButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "openSessionButton" );
		_openSessionButton->setData( "Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_openSessionButton, 1 );

		_openRecentSessionButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "openRecentSessionButton" );
		_openRecentSessionButton->setData(
			"Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_openRecentSessionButton, 1 );

		_saveSessionButton = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "saveSessionButton" );
		_saveSessionButton->setData( "Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_saveSessionButton, 2 );

		_saveAsSessionButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "saveAsSessionButton" );
		_saveAsSessionButton->setData( "Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_saveAsSessionButton, 2 );

		_loadSettingsButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "loadSettingsButton" );
		_loadSettingsButton->setData( "Load settings", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_loadSettingsButton, 3 );

		_saveSettingsButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "saveSettingsButton" );
		_saveSettingsButton->setData(
			"Save settings", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_saveSettingsButton, 3 );

		_recentSessionMenu = new QMenu( _openRecentSessionButton );
		_refreshRecentFiles();
		_openRecentSessionButton->setMenu( _recentSessionMenu );

		validate();
	}
	void MenuHomeSessionWidget::_setupSlots()
	{
		_newSessionButton->setTriggerAction( this, &MenuHomeSessionWidget::_newSession );
		_openSessionButton->setTriggerAction( this, &MenuHomeSessionWidget::_openFile );
		_saveSessionButton->setTriggerAction( this, &MenuHomeSessionWidget::_saveSession );
		_saveAsSessionButton->setTriggerAction( this, &MenuHomeSessionWidget::_saveAsSession );
		_loadSettingsButton->setTriggerAction( this, &MenuHomeSessionWidget::_loadSettings );
		_saveSettingsButton->setTriggerAction( this, &MenuHomeSessionWidget::_saveSettings );
	}
	void MenuHomeSessionWidget::localize() {}

	void MenuHomeSessionWidget::_refreshRecentFiles()
	{
		_recentSessionMenu->clear();

		int actionIndex = 0;

		for ( const VTX::FilePath * const recentFile : Setting::recentLoadingPath )
		{
			CustomWidget::IndexedAction * const action
				= new CustomWidget::IndexedAction( actionIndex, _recentSessionMenu );
			const QString path = QString::fromStdString( recentFile->string() );
			action->setText( path );

			connect( action,
					 &CustomWidget::IndexedAction::triggeredWithIndex,
					 this,
					 &MenuHomeSessionWidget::_loadRecentSession );

			_recentSessionMenu->addAction( action );
			actionIndex++;
		}

		_openRecentSessionButton->setEnabled( actionIndex > 0 );
	}

	void MenuHomeSessionWidget::_newSession() { VTX_ACTION( new Action::Main::New() ); }
	void MenuHomeSessionWidget::_openFile() { Dialog::openLoadSessionDialog(); }
	void MenuHomeSessionWidget::_saveSession() const
	{
		FilePath * const filePath = new FilePath( VTXApp::get().getCurrentPath() );
		if ( filePath->empty() )
			Dialog::openSaveSessionDialog();
		else
			VTX_ACTION( new Action::Main::Save( filePath ) );
	}
	void MenuHomeSessionWidget::_saveAsSession() const { Dialog::openSaveSessionDialog(); }

	void MenuHomeSessionWidget::_loadRecentSession( const int & p_ptrSessionIndex ) const
	{
		FilePath * const path = new FilePath( *Setting::getRecentLoadingPath( p_ptrSessionIndex ) );
		VTX_ACTION( new Action::Main::Open( path ) );
	}

	void MenuHomeSessionWidget::_loadSettings() const { VTX_ACTION( new Action::Setting::Load() ); }

	void MenuHomeSessionWidget::_saveSettings() const { VTX_ACTION( new Action::Setting::Save() ); }

} // namespace VTX::UI::Widget::MainMenu::Home
