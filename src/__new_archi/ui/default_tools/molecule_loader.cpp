#include "molecule_loader.hpp"
#include "__new_archi/ui/default_tools/keys.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/main_window.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_tooltab_widget.hpp"
#include "__new_archi/ui/qt/widget/scene/view/molecule_scene_view.hpp"
#include "__new_archi/ui/qt/widget/scene/view/path_scene_view.hpp"
#include "__new_archi/ui/qt/widget_factory.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "ui/widget/custom_widget/indexed_action.hpp"
#include <QMenu>
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::DefaultTools
{
	SessionLoader::SessionLoader() {}

	void SessionLoader::instantiateTool()
	{
		_addButtonsInMainMenu();
		_addActionsInContextualMenus();
	}

	void SessionLoader::_addButtonsInMainMenu()
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

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const downloadMoleculeButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "downloadMoleculeButton" );
		downloadMoleculeButton->setData(
			"Download", ":/sprite/download_molecule_icon.png", Qt::Orientation::Horizontal );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const openSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "openSessionButton" );
		openSessionButton->setData( "Open", ":/sprite/open_session_icon.png", Qt::Orientation::Horizontal );

		_openRecentSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "openRecentSessionButton" );
		_openRecentSessionButton->setData(
			"Recent", ":/sprite/openrecent_session_icon.png", Qt::Orientation::Horizontal );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const saveSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "saveSessionButton" );
		saveSessionButton->setData( "Save", ":/sprite/save_session_icon.png", Qt::Orientation::Horizontal );

		VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget * const saveAsSessionButton
			= QT::WidgetFactory::get().instantiateWidget<VTX::UI::QT::Widget::MainMenu::MenuToolButtonWidget>(
				&tooltab, "saveAsSessionButton" );
		saveAsSessionButton->setData( "Save as...", ":/sprite/saveas_session_icon.png", Qt::Orientation::Horizontal );

		_recentSessionMenu = new QMenu( _openRecentSessionButton );
		_refreshRecentFiles();
		_openRecentSessionButton->setMenu( _recentSessionMenu );

		toolBlock.pushButton( *newSessionButton );
		toolBlock.pushButton( *downloadMoleculeButton, *openSessionButton, *_openRecentSessionButton );
		toolBlock.pushButton( *saveSessionButton, *saveAsSessionButton );

		mainWindow->getMainMenu().getTab( "Main" ).getToolBlock( "File" );
	}

	void SessionLoader::_addActionsInContextualMenus()
	{
		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		QT::Widget::ContextualMenu::BaseContextualMenu * const sceneContextualMenu
			= mainWindow->getContextualMenu().getMenu( DefaultTools::ContextualMenu::SCENE );

		sceneContextualMenu->appendToSection( "Loading", "Load Molecule", this, &SessionLoader::_openFile );
		sceneContextualMenu->appendToSection(
			"Loading", "Download Molecule", this, &SessionLoader::_downloadMoleculeFile );
	}

	void SessionLoader::_refreshRecentFiles()
	{
		_recentSessionMenu->clear();

		int actionIndex = 0;

		for ( const Util::FilePath & recentFile : Setting::recentLoadingPath )
		{
			VTX::UI::Widget::CustomWidget::IndexedAction * const action
				= new VTX::UI::Widget::CustomWidget::IndexedAction( actionIndex, _recentSessionMenu );
			action->setText( QString::fromStdString( recentFile.path() ) );

			connect( action,
					 &VTX::UI::Widget::CustomWidget::IndexedAction::triggeredWithIndex,
					 this,
					 &SessionLoader::_loadRecentSession );

			_recentSessionMenu->addAction( action );
			actionIndex++;
		}

		_openRecentSessionButton->setEnabled( actionIndex > 0 );
	}
	void SessionLoader::_newSession() const { Dialog::createNewSessionDialog(); }
	void SessionLoader::_downloadMoleculeFile() const { UI::Dialog::openDownloadMoleculeDialog(); }
	void SessionLoader::_openFile() const { Dialog::openLoadSessionDialog(); }
	void SessionLoader::_saveSession() const
	{
		VTX_ACTION( new Action::Main::Save( VTXApp::get().getScenePathData().getCurrentPath() ) );
	}
	void SessionLoader::_saveAsSession() const { Dialog::openSaveSessionDialog(); }

	void SessionLoader::_loadRecentSession( const int & p_ptrSessionIndex ) const
	{
		const Util::FilePath * const recentPath = Setting::getRecentLoadingPath( p_ptrSessionIndex );

		if ( recentPath == nullptr )
		{
			return;
		}

		VTX_ACTION( new Action::Main::Open( *recentPath ) );
	}

} // namespace VTX::UI::DefaultTools
