#include "session_tool.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/tool/keys.hpp"
#include "qt/tool/session/action.hpp"
#include "qt/tool/session/dialog.hpp"
#include "qt/tool/session/widget/dialog/download_molecule_dialog.hpp"
#include "qt/widget/custom_widget/indexed_action.hpp"
#include "qt/widget/main_menu/menu_tooltab_widget.hpp"
#include "qt/widget_factory.hpp"
#include <QMenu>
#include <QVBoxLayout>
#include <QWidget>
#include <old/action/action_manager.hpp>
#include <old/action/setting.hpp>
#include <old/io/struct/scene_path_data.hpp>
#include <old/model/molecule.hpp>
#include <old/model/path.hpp>
#include <old/vtx_app.hpp>

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
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		const VTX::UI::Core::ToolLayoutData & layoutData = getLayoutData();

		VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget & tooltab
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuTooltabWidget &>(
				VTX::UI::QT::QT_APP()->getMainWindow().getMainMenu().getTab( layoutData.tabName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock
			= dynamic_cast<VTX::UI::QT::Widget::MainMenu::MenuToolBlockWidget &>(
				tooltab.getToolBlock( layoutData.blockName ) );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const newSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "newSessionButton" );
		newSessionButton->setData( "New", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
		newSessionButton->setTriggerAction( this, &SessionTool::_newSession );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const downloadMoleculeButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "downloadMoleculeButton" );
		downloadMoleculeButton->setData(
			"Download", ":/sprite/download_molecule_icon.png", Qt::Orientation::Horizontal );
		downloadMoleculeButton->setTriggerAction( this, &SessionTool::_downloadMoleculeFile );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const openSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "openSessionButton" );
		openSessionButton->setData( "Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );
		openSessionButton->setTriggerAction( this, &SessionTool::_openFile );

		_openRecentSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "openRecentSessionButton" );
		_openRecentSessionButton->setData(
			"Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const saveSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "saveSessionButton" );
		saveSessionButton->setData( "Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );
		saveSessionButton->setTriggerAction( this, &SessionTool::_saveSession );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const saveAsSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "saveAsSessionButton" );
		saveAsSessionButton->setData( "Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );
		saveAsSessionButton->setTriggerAction( this, &SessionTool::_saveAsSession );

		_recentSessionMenu = new QMenu( _openRecentSessionButton );
		_refreshRecentFiles();
		_openRecentSessionButton->setMenu( _recentSessionMenu );

		toolBlock.pushButton( *newSessionButton );
		toolBlock.pushButton( *downloadMoleculeButton, *openSessionButton, *_openRecentSessionButton );
		toolBlock.pushButton( *saveSessionButton, *saveAsSessionButton );

		mainWindow->getMainMenu().getTab( "Main" ).getToolBlock( "File" );
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
		}

		QT::Widget::ContextualMenu::BaseContextualMenu * const sceneContextualSelection
			= mainWindow->getContextualMenu().getMenu( Tool::ContextualMenu::SELECTION );

		if ( sceneContextualSelection != nullptr )
		{
			sceneContextualMenu->appendToSection( "Loading", "Load Trajectory", this, &SessionTool::_openFile );
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

		for ( const Util::FilePath & recentFile : Setting::recentLoadingPath )
		{
			QT::Widget::CustomWidget::IndexedAction * const action
				= new QT::Widget::CustomWidget::IndexedAction( actionIndex, _recentSessionMenu );
			action->setText( QString::fromStdString( recentFile.path() ) );

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
		VTX_ACTION( new Session::Action::Save( VTXApp::get().getScenePathData().getCurrentPath() ) );
	}
	void SessionTool::_saveAsSession() const { Session::Dialog::openSaveSessionDialog(); }

	void SessionTool::_loadRecentSession( const int & p_ptrSessionIndex ) const
	{
		const Util::FilePath * const recentPath = Setting::getRecentLoadingPath( p_ptrSessionIndex );

		if ( recentPath == nullptr )
		{
			return;
		}

		VTX_ACTION( new Session::Action::Open( *recentPath ) );
	}

} // namespace VTX::UI::QT::Tool
