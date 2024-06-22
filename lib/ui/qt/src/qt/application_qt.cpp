#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/style.hpp"
#include "qt/widget/renderer/dialog.hpp"
#include "qt/widget_factory.hpp"
#include <QCoreApplication>
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
	/*
	void ApplicationQt::configure()
	{
		// Setup some Qt static configuration.
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );
	}
	*/

	int ZERO = 0;
	ApplicationQt::ApplicationQt() : UI::Core::BaseUIApplication() //, QApplication( ZERO, nullptr )
	{
		VTX_DEBUG( "ApplicationQt::ApplicationQt()" );
		// connect( this, &QCoreApplication::aboutToQuit, this, &ApplicationQt::stop );
	}
	ApplicationQt::~ApplicationQt() {}

	/*
	void ApplicationQt::init()
	{
		const FilePath path = VTX::Util::Filesystem::getExecutableDir() / "logs";
		std::filesystem::create_directory( path );
		VTX::Util::Logger::get().init( path );

		UI::Core::BaseUIApplication::init();

		//_currentMode = std::make_unique<App::Mode::Visualization>();
	}
	*/

	void ApplicationQt::start( const std::vector<std::string> & p_args )
	{
		UI::Core::BaseUIApplication::start( p_args );
		//_currentMode->enter();

		//_returnCode = exec();
	}

	void ApplicationQt::_initUI( const std::vector<std::string> & p_args )
	{
		// Internal::initSettings( App::SETTINGS() );

		//// Init Modes.
		// App::Core::init( dynamic_cast<App::Mode::Visualization &>( *_currentMode ) );
		//_currentMode->enter();

		// Create UI.
		_initQt();

		_instantiateMainWindow();

		_mainWindow->getMainMenu().setCurrentTab( 0 );
		//_renderWidget->setFocus();
	}

	void ApplicationQt::_initQt()
	{
		/*		setWindowIcon( QIcon( ":/sprite/logo.png" ) );

		setStyle( QString::fromStdString( Style::DEFAULT_STYLE_FACTORY ) );

		QPalette appPalette = palette();
		Style::applyApplicationPaletteInPalette( appPalette );
		setPalette( appPalette );
		*/

#ifdef _DEBUG
		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
#endif
	}

	void ApplicationQt::_instantiateMainWindow()
	{
		_mainWindow = WidgetFactory::get().instantiateWidget<QT::MainWindow>( nullptr, "MainWindow" );
	}

	void ApplicationQt::_startUI( const std::vector<std::string> & p_args )
	{
		_mainWindow->show();
		_mainWindow->initWindowLayout();
	}

	void ApplicationQt::stop()
	{
		// TODO BaseUIApplication::stop() called here because some model are strongly linked to _gl
		BaseUIApplication::stop();

		if ( _mainWindow != nullptr )
		{
			delete _mainWindow;
		}

		//_currentMode = nullptr;
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

	// App::Mode::BaseMode & MODE() { return QT_APP()->getCurrentMode(); }

} // namespace VTX::UI::QT
