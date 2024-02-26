#include "ui/qt/tool/session_tool.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/keys.hpp"
#include "ui/qt/tool/session/action.hpp"
#include "ui/qt/tool/session/dialog.hpp"
#include "ui/qt/tool/session/widget/dialog/download_molecule_dialog.hpp"
#include "ui/qt/widget/custom_widget/indexed_action.hpp"
#include "ui/qt/widget/main_menu/menu_tooltab_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QMenu>
#include <QVBoxLayout>
#include <QWidget>

#include <app/old/action/scene.hpp>
#include <app/old/action/setting.hpp>
#include <app/old/internal/io/serialization/scene_path_data.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/video/path.hpp>
#include <app/old/vtx_app.hpp>

namespace VTX::UI::QT::Tool
{
	SessionTool::SessionTool() : BaseQtTool() {}

	void SessionTool::instantiateTool()
	{
		_addButtonsInMainMenu();
		_addActionsInContextualMenus();
		_registerShortcuts();
	}

	void SessionTool::_addButtonsInMainMenu()
	{
		QT::MainWindow * const						mainWindow = &QT::QT_APP()->getMainWindow();
		QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock  = mainWindow->getMainMenuToolBlock( getLayoutData() );

		QT::Widget::MainMenu::MenuToolButtonWidget * const newSessionButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "newSessionButton" );
		newSessionButton->setData( "New", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
		newSessionButton->setTriggerAction( this, &SessionTool::_newSession );

		QT::Widget::MainMenu::MenuToolButtonWidget * const downloadMoleculeButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "downloadMoleculeButton" );
		downloadMoleculeButton->setData(
			"Download", ":/sprite/download_molecule_icon.png", Qt::Orientation::Horizontal );
		downloadMoleculeButton->setTriggerAction( this, &SessionTool::_downloadMoleculeFile );

		QT::Widget::MainMenu::MenuToolButtonWidget * const openSessionButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "openSessionButton" );
		openSessionButton->setData( "Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
		openSessionButton->setTriggerAction( this, &SessionTool::_openFile );

		_openRecentSessionButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "openRecentSessionButton" );
		_openRecentSessionButton->setData(
			"Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );

		QT::Widget::MainMenu::MenuToolButtonWidget * const saveSessionButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "saveSessionButton" );
		saveSessionButton->setData( "Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );
		saveSessionButton->setTriggerAction( this, &SessionTool::_saveSession );

		QT::Widget::MainMenu::MenuToolButtonWidget * const saveAsSessionButton
			= QT::WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
				&toolBlock, "saveAsSessionButton" );
		saveAsSessionButton->setData( "Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
		saveAsSessionButton->setTriggerAction( this, &SessionTool::_saveAsSession );

		_recentSessionMenu = new QMenu( _openRecentSessionButton );
		_refreshRecentFiles();
		_openRecentSessionButton->setMenu( _recentSessionMenu );

		toolBlock.pushButton( *newSessionButton );
		toolBlock.pushButton( *downloadMoleculeButton, *openSessionButton, *_openRecentSessionButton );
		toolBlock.pushButton( *saveSessionButton, *saveAsSessionButton );
	}
	void SessionTool::_addActionsInContextualMenus()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::ContextualMenu::BaseContextualMenu * const sceneContextualMenu
			= mainWindow->getContextualMenu().getMenu( Tool::ContextualMenu::SCENE );

		if ( sceneContextualMenu != nullptr )
		{
			sceneContextualMenu->appendToSection( "Loading", "Load Molecule", this, &SessionTool::_openFile );
			sceneContextualMenu->appendToSection(
				"Loading", "Download Molecule", this, &SessionTool::_downloadMoleculeFile );

			sceneContextualMenu->appendToSection( "Session", "Save Session", this, &SessionTool::_saveSession );
			sceneContextualMenu->appendToSection( "Session", "Save Session As...", this, &SessionTool::_saveAsSession );

			sceneContextualMenu->appendToSection( "Session", "Clear", this, &SessionTool::_saveAsSession );
		}

		QT::Widget::ContextualMenu::BaseContextualMenu * const selectionContextualMenu
			= mainWindow->getContextualMenu().getMenu( Tool::ContextualMenu::SELECTION );

		if ( selectionContextualMenu != nullptr )
		{
			selectionContextualMenu->appendToSection( "Loading", "Load Trajectory", this, &SessionTool::_openFile );
		}
	}
	void SessionTool::_registerShortcuts()
	{
		QT_APP()->getMainWindow().addShortcut( "Ctrl+N", this, &SessionTool::_newSession );
		QT_APP()->getMainWindow().addShortcut( "Ctrl+O", this, &SessionTool::_openFile );
		QT_APP()->getMainWindow().addShortcut( "Ctrl+S", this, &SessionTool::_saveSession );
		QT_APP()->getMainWindow().addShortcut( "Ctrl+Shift+S", this, &SessionTool::_saveAsSession );
	}

	void SessionTool::_refreshRecentFiles()
	{
		_recentSessionMenu->clear();

		int actionIndex = 0;

		for ( const FilePath & recentFile : VTX::App::Old::Application::Setting::recentLoadingPath )
		{
			QT::Widget::CustomWidget::IndexedAction * const action
				= new QT::Widget::CustomWidget::IndexedAction( actionIndex, _recentSessionMenu );
			action->setText( QString::fromStdString( recentFile.string() ) );

			connect( action,
					 &QT::Widget::CustomWidget::IndexedAction::triggeredWithIndex,
					 this,
					 &SessionTool::_loadRecentSession );

			_recentSessionMenu->addAction( action );
			actionIndex++;
		}

		_openRecentSessionButton->setEnabled( actionIndex > 0 );
	}
	void SessionTool::_newSession() const { Session::Dialog::createNewSessionDialog(); }
	void SessionTool::_downloadMoleculeFile() const { Session::Dialog::openDownloadMoleculeDialog(); }
	void SessionTool::_openFile() const { Session::Dialog::openLoadSessionDialog(); }
	void SessionTool::_saveSession() const
	{
		VTX_ACTION( new Session::Action::Save( App::Old::VTXApp::get().getScenePathData().getCurrentPath() ) );
	}
	void SessionTool::_saveAsSession() const { Session::Dialog::openSaveSessionDialog(); }
	void SessionTool::_clearSession() const { VTX_ACTION( new VTX::App::Old::Action::Scene::ResetScene() ); }

	void SessionTool::_loadRecentSession( const int & p_ptrSessionIndex ) const
	{
		const FilePath * const recentPath = VTX::App::Old::Application::Setting::getRecentLoadingPath( p_ptrSessionIndex );

		if ( recentPath == nullptr )
		{
			return;
		}

		VTX_ACTION( new Session::Action::Open( *recentPath ) );
	}

} // namespace VTX::UI::QT::Tool