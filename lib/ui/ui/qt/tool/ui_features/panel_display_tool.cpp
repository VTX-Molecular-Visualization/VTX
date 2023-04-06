#include "panel_display_tool.hpp"
#include "core/define.hpp"
#include "qt/action/main.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/util.hpp"
#include "qt/widget/main_menu/menu_toolblock_widget.hpp"
#include "qt/widget/main_menu/menu_tooltab_widget.hpp"
#include "qt/widget_factory.hpp"
#include <QMenu>
#include <QWidget>
#include <list>
#include <old/action/action_manager.hpp>
#include <old/action/setting.hpp>

namespace VTX::UI::QT::Tool
{
	PanelDisplayTool::PanelDisplayTool() : BaseQtTool()
	{
		_registerEvent( Event::Global::UI_APPLICATION_INITIALIZED );
		_registerEvent( Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE );
	}

	void PanelDisplayTool::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::UI_APPLICATION_INITIALIZED )
		{
			QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

			_panelListMenu = new QMenu( _panelListButton );
			_populatePanelMenu();

			_panelListButton->setMenu( _panelListMenu );
			connect( _panelListMenu, &QMenu::triggered, this, &PanelDisplayTool::_togglePanelVisibility );

			_refreshPanelVisibilityState();
		}
		else if ( p_event.name == Event::Global::DOCK_WINDOW_VISIBILITY_CHANGE )
		{
			_refreshPanelVisibilityState();
		}
	}

	void PanelDisplayTool::instantiateTool() { _addButtonsInMainMenu(); }

	void PanelDisplayTool::_addButtonsInMainMenu()
	{
		QT::MainWindow * const						mainWindow = &QT::QT_APP()->getMainWindow();
		QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock  = mainWindow->getMainMenuToolBlock( getLayoutData() );

		_panelListButton = WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>(
			&toolBlock, "panelListButton" );
		_panelListButton->setData( "Panels", ":/sprite/windows_icon.png", Qt::Orientation::Vertical );

		toolBlock.pushButton( *_panelListButton );
	}

	void PanelDisplayTool::_populatePanelMenu()
	{
		std::map<std::string, std::map<std::string, const QT::QtPanel *>> mapSectionPanels
			= std::map<std::string, std::map<std::string, const QT::QtPanel *>>();

		QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

		for ( auto panelPair : mainWindow->getPanelMap() )
		{
			const QT::QtPanel * qtPanel = static_cast<const QT::QtPanel *>( panelPair.second );

			if ( qtPanel->referenceInPanels )
			{
				mapSectionPanels[ qtPanel->section ][ qtPanel->name ] = qtPanel;
			}
		}

		for ( auto pairSectionPanels : mapSectionPanels )
		{
			_panelListMenu->addSection( QString::fromStdString( pairSectionPanels.first ) );

			for ( auto pairNamePanel : pairSectionPanels.second )
			{
				QAction * const action = new QAction( _panelListMenu );
				action->setText( QString::fromStdString( pairNamePanel.second->name ) );
				const Core::WidgetKey & key = mainWindow->findKeyFromPanel( pairNamePanel.second );
				action->setData( QT::Util::widgetKeyToQVariant( key ) );
				action->setCheckable( true );

				_panelListMenu->addAction( action );
			}
		}
	}
	void PanelDisplayTool::_togglePanelVisibility( const QAction * const p_action )
	{
		QT::MainWindow & mainWindow = QT::QT_APP()->getMainWindow();

		const Core::WidgetKey & panelKey = QT::Util::QVariantTowWidgetKey( p_action->data() );

		QT::QtPanel * const qtPanel = mainWindow.getPanel<QT::QtPanel>( panelKey );
		qtPanel->setVisible( !qtPanel->isVisible() );
	}
	void PanelDisplayTool::_refreshPanelVisibilityState()
	{
		QT::MainWindow & mainWindow = QT::QT_APP()->getMainWindow();

		const bool previousBlockSignalState = _panelListMenu->blockSignals( true );

		for ( QAction * const action : _panelListMenu->actions() )
		{
			const Core::WidgetKey & panelKey = QT::Util::QVariantTowWidgetKey( action->data() );

			if ( panelKey == Core::INVALID_WIDGET_KEY )
				continue;

			QT::QtPanel * const qtPanel = mainWindow.getPanel<QT::QtPanel>( panelKey );
			action->setChecked( qtPanel->isVisible() );
		}

		_panelListMenu->blockSignals( previousBlockSignalState );
	}

} // namespace VTX::UI::QT::Tool
