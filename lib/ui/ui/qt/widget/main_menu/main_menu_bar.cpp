#include "main_menu_bar.hpp"
#include "qt/widget_factory.hpp"
#include "menu_tooltab_widget.hpp"
#include <QFont>

namespace VTX::UI::QT::Widget::MainMenu
{
	MainMenuBar::MainMenuBar( QWidget * p_parent ) : Core::MainMenu::MainMenuBar(), BaseManualWidget( p_parent ) {}

	Core::MainMenu::MainMenuTooltab * MainMenuBar::_instantiateTab() const
	{
		MenuTooltabWidget * const tooltab
			= WidgetFactory::get().instantiateWidget<MenuTooltabWidget>( _tabWidget, "main_menu_tool_tab" );
		return tooltab;
	}

	void MainMenuBar::_addTab( const Core::WidgetKey & p_tabID, Core::MainMenu::MainMenuTooltab * const p_tab )
	{
		Core::MainMenu::MainMenuBar::_addTab( p_tabID, p_tab );

		QT::Widget::MainMenu::MenuTooltabWidget * const qtToolTab
			= dynamic_cast<QT::Widget::MainMenu::MenuTooltabWidget *>( p_tab );
		_tabWidget->addTab( qtToolTab, QString::fromStdString( p_tabID ) );
	}

	void MainMenuBar::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setFixedHeight( 138 );

		_tabWidget = new QTabWidget( this );
		_tabWidget->setObjectName( "tabWidget" );
		_tabWidget->setFixedSize( size() );

		QFont menuBarFont;
		menuBarFont.setPointSize( 10 );
		_tabWidget->setFont( menuBarFont );

		//_mainMenu = WidgetFactory::get().instantiateWidget<Home::MenuHomeWidget>( _tabWidget, "mainMenu" );
		//_tabWidget->addTab( _mainMenu, "Home" );
		//_mapTabs[ "Home" ] = _mainMenu;

		//_viewMenu = WidgetFactory::get().instantiateWidget<Visualization::MenuVisualizationWidget>(
		//	_tabWidget, "visualizationMenu" );
		//_tabWidget->addTab( _viewMenu, "Visualization" );
		//_mapTabs[ "Visualization" ] = _viewMenu;

		//_toolMenu = WidgetFactory::get().instantiateWidget<Tool::MenuToolWidget>( _tabWidget, "toolMenu" );
		//_tabWidget->addTab( _toolMenu, "Tools" );
		//_mapTabs[ "Tools" ] = _toolMenu;

		// !V0.1
		//_movieMenu = new QLabel( "movieMenu", this );
		//_tabWidget->addTab( _movieMenu, "Movie" );

		//_extensionsMenu
		//	= WidgetFactory::get().instantiateWidget<Extensions::MenuExtensionsWidget>( this, "extensionsMenu" );
		//_tabWidget->addTab( _extensionsMenu, "Modules" );
	}

	void MainMenuBar::resizeEvent( QResizeEvent * p_event )
	{
		// Manually resize tabWidget width (SizePolicy doesn't seems to works).
		_tabWidget->setFixedWidth( p_event->size().width() );
	}

	void MainMenuBar::_setupSlots() {}
	void MainMenuBar::localize() {}

} // namespace VTX::UI::QT::Widget::MainMenu
