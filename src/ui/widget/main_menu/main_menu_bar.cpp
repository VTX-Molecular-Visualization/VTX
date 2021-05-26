#include "main_menu_bar.hpp"
#include "ui/widget_factory.hpp"
#include <QFont>

namespace VTX::UI::Widget::MainMenu
{
	MainMenuBar::MainMenuBar( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void MainMenuBar::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setGeometry( QRect( 0, 2, 800, 130 ) );
		setMinimumSize( 800, 130 );
		setContentsMargins( 10, 2, 10, 2 );
		setSizePolicy( QSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed ) );

		_tabWidget = new QTabWidget( this );
		_tabWidget->setObjectName( "tabWidget" );

		_tabWidget->setSizePolicy( QSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed ) );
		_tabWidget->setFixedHeight( 130 );

		QFont menuBarFont;
		menuBarFont.setPointSize( 10 );
		_tabWidget->setFont( menuBarFont );

		_mainMenu = WidgetFactory::get().instantiateWidget<Home::MenuHomeWidget>( this, "mainMenu" );
		_tabWidget->addTab( _mainMenu, "Main" );

		_viewMenu = WidgetFactory::get().instantiateWidget<Visualization::MenuVisualizationWidget>(
			this, "visualizationMenu" );
		_tabWidget->addTab( _viewMenu, "Visualization" );

		// !V0.1
		//_movieMenu = new QLabel( "movieMenu", this );
		//_tabWidget->addTab( _movieMenu, "Movie" );

		//_extensionsMenu
		//	= WidgetFactory::get().instantiateWidget<Extensions::MenuExtensionsWidget>( this, "extensionsMenu" );
		//_tabWidget->addTab( _extensionsMenu, "Modules" );

		_tabWidget->setCurrentIndex( 0 );
	}

	void MainMenuBar::_setupSlots() {}
	void MainMenuBar::localize() {}
} // namespace VTX::UI::Widget::MainMenu
