#include "main_menu_bar.hpp"
#include "ui/widget_factory.hpp"
#include <QFont>

namespace VTX::UI::Widget::MainMenu
{
	MainMenuBar::MainMenuBar( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

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

		_mainMenu = WidgetFactory::get().instantiateWidget<Home::MenuHomeWidget>( _tabWidget, "mainMenu" );
		_tabWidget->addTab( _mainMenu, "Home" );
		_mapTabs[ "Home" ] = _mainMenu;

		_viewMenu = WidgetFactory::get().instantiateWidget<Visualization::MenuVisualizationWidget>(
			_tabWidget, "visualizationMenu" );
		_tabWidget->addTab( _viewMenu, "Visualization" );
		_mapTabs[ "Visualization" ] = _viewMenu;

		_toolMenu = WidgetFactory::get().instantiateWidget<Tool::MenuToolWidget>( _tabWidget, "toolMenu" );
		_tabWidget->addTab( _toolMenu, "Tools" );
		_mapTabs[ "Tools" ] = _toolMenu;

		// !V0.1
		//_movieMenu = new QLabel( "movieMenu", this );
		//_tabWidget->addTab( _movieMenu, "Movie" );

		//_extensionsMenu
		//	= WidgetFactory::get().instantiateWidget<Extensions::MenuExtensionsWidget>( this, "extensionsMenu" );
		//_tabWidget->addTab( _extensionsMenu, "Modules" );

		_tabWidget->setCurrentIndex( 0 );
	}

	void MainMenuBar::resizeEvent( QResizeEvent * p_event )
	{
		// Manually resize tabWidget width (SizePolicy doesn't seems to works).
		_tabWidget->setFixedWidth( p_event->size().width() );
	}

	void MainMenuBar::_setupSlots() {}
	void MainMenuBar::localize() {}

	MenuTooltabWidget & MainMenuBar::getTab( const std::string & p_tabName )
	{
		map_tab::const_iterator it = _mapTabs.find( p_tabName );

		MenuTooltabWidget * res;

		if ( it == _mapTabs.end() )
		{
			res = WidgetFactory::get().instantiateWidget<MenuTooltabWidget>( _tabWidget, "mainMenu" );
			_tabWidget->addTab( _mainMenu, QString::fromStdString( p_tabName ) );
		}
		else
		{
			res = it->second;
		}

		return *res;
	}

} // namespace VTX::UI::Widget::MainMenu
