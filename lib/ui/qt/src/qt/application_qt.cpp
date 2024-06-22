#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/style.hpp"
#include "qt/widget/renderer/dialog.hpp"
#include "qt/widget_factory.hpp"
#include <QApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QPalette>
#include <QStyleFactory>
#include <app/application/system/settings_system.hpp>
#include <app/core/init_settings.hpp>
#include <app/core/init_visualization_mode.hpp>
#include <app/mode/base_mode.hpp>
#include <app/mode/visualization.hpp>
#include <util/logger.hpp>
#include <util/math/transform.hpp>

namespace VTX::UI::QT
{
	int ZERO = 0;
	ApplicationQt::ApplicationQt() : UI::Core::BaseUIApplication() //, QApplication( ZERO, nullptr )
	{
		VTX_DEBUG( "ApplicationQt::ApplicationQt()" );
	}

	ApplicationQt::~ApplicationQt()
	{
		if ( _mainWindow != nullptr )
		{
			delete _mainWindow;
		}
	}

	void ApplicationQt::_init( const std::vector<std::string> & p_args )
	{
		// Setup some Qt static configuration.
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );

		// Internal::initSettings( App::SETTINGS() );

		//// Init Modes.
		// App::Core::init( dynamic_cast<App::Mode::Visualization &>( *_currentMode ) );
		//_currentMode->enter();

		/*		setWindowIcon( QIcon( ":/sprite/logo.png" ) );

		setStyle( QString::fromStdString( Style::DEFAULT_STYLE_FACTORY ) );

		QPalette appPalette = palette();
		Style::applyApplicationPaletteInPalette( appPalette );
		setPalette( appPalette );
		*/

		_qApplication = new QApplication( ZERO, nullptr );
		_qApplication->connect( _qApplication, &QApplication::aboutToQuit, [ this ]() { BaseUIApplication::stop(); } );

		_mainWindow = WidgetFactory::get().instantiateWidget<QT::MainWindow>( nullptr, "MainWindow" );
	}

	void ApplicationQt::_build( const UI::Core::LayoutDescriptor & p_layout ) {}

	void ApplicationQt::_start( const std::vector<std::string> & p_args )
	{
		_mainWindow->getMainMenu().setCurrentTab( 0 );
		_mainWindow->show();
		_mainWindow->initWindowLayout();
		//_renderWidget->setFocus();

		// TODO: return code?
		_qApplication->exec();
	}

	/*
	bool ApplicationQt::notify( QObject * const receiver, QEvent * const event )
	{
		try
		{
			return QApplication::notify( receiver, event );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
#ifdef VTX_PRODUCTION
			UI::Dialog::unhandledException();
#endif
			return true;
		}
	}
	*/

	void ApplicationQt::softQuit()
	{
		// closeAllWindows();
	}

} // namespace VTX::UI::QT
