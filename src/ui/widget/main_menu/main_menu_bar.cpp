#include "main_menu_bar.hpp"
// !V0.1
// #include "extensions/menu_extensions_widget.hpp"
#include "camera/toolbar.hpp"
#include "home/menu_home_widget.hpp"
#include "molecule/toolbar.hpp"
#include "tool/menu_tool_widget.hpp"
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

		Home::MenuHomeWidget * const mainMenu
			= WidgetFactory::get().instantiateWidget<Home::MenuHomeWidget>( _tabWidget, "mainMenu" );
		_tabWidget->addTab( mainMenu, "File" );

		// Visualization::MenuVisualizationWidget * const viewMenu
		//	= WidgetFactory::get().instantiateWidget<Visualization::MenuVisualizationWidget>( _tabWidget,
		//																					  "visualizationMenu" );
		//_tabWidget->addTab( viewMenu, "Visualization" );

		Molecule::Toolbar * const moleculeMenu
			= WidgetFactory::get().instantiateWidget<Molecule::Toolbar>( _tabWidget, "moleculeMenu" );
		_tabWidget->addTab( moleculeMenu, "Edit" );

		Camera::Toolbar * const cameraMenu
			= WidgetFactory::get().instantiateWidget<Camera::Toolbar>( _tabWidget, "cameraMenu" );
		_tabWidget->addTab( cameraMenu, "Visualization" );

		Tool::MenuToolWidget * const toolMenu
			= WidgetFactory::get().instantiateWidget<Tool::MenuToolWidget>( _tabWidget, "toolMenu" );
		_tabWidget->addTab( toolMenu, "Tools" );

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
} // namespace VTX::UI::Widget::MainMenu
