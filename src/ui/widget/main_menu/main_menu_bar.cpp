#include "main_menu_bar.hpp"
#include "ui/widget_factory.hpp"
#include <QFont>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				MainMenuBar::MainMenuBar( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

				void MainMenuBar::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					setGeometry( QRect( 0, 2, 800, 130 ) );
					setMinimumSize( 800, 130 );
					setContentsMargins( 10, 2, 10, 2 );
					setSizePolicy( QSizePolicy( QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Fixed ) );

					_tabWidget = new QTabWidget( this );
					_tabWidget->setObjectName( "tabWidget" );

					_tabWidget->setSizePolicy( QSizePolicy( QSizePolicy::Policy::Maximum, QSizePolicy::Policy::Maximum ) );

					QFont menuBarFont;
					menuBarFont.setPointSize( 10 );
					_tabWidget->setFont( menuBarFont );

					_mainMenu = WidgetFactory::get().GetWidget<Home::MenuHomeWidget>( this, "mainMenu" );
					_tabWidget->addTab( _mainMenu, "Main" );

					_viewMenu = WidgetFactory::get().GetWidget<Visualization::MenuVisualizationWidget>( this, "visualizationMenu" );
					_tabWidget->addTab( _viewMenu, "Visualization" );

					_movieMenu = new QLabel( "movieMenu", this );
					_tabWidget->addTab( _movieMenu, "Movie" );

					_tabWidget->setCurrentIndex( 0 );
				}

				void MainMenuBar::_setupSlots() {}
				void MainMenuBar::localize() {}
			} // namespace MainMenu
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
