#include "ui/qt/application_qt.hpp"
// #include "ui/dialog.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/tool/render/dialog.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QCoreApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QPalette>
#include <QStyleFactory>
#include <app/event/global.hpp>
// #include <exception>
#include <util/logger.hpp>
// #include <app/action/main.hpp>
#include <app/event.hpp>
#include <app/old_app/generic/base_opengl.hpp>
#include <app/vtx_app.hpp>
// #include <app/vtx_app.hpp>

namespace VTX::UI::QT
{
	void ApplicationQt::configure()
	{
		// Setup some Qt static configuration.
		QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );
		QCoreApplication::setAttribute( Qt::AA_DontCheckOpenGLContextThreadAffinity );

		// Init resources
		Q_INIT_RESOURCE( resources_ui );
	}

	int ZERO = 0;
	ApplicationQt::ApplicationQt() : Core::BaseUIApplication(), QApplication( ZERO, nullptr )
	{
		connect( this, &QCoreApplication::aboutToQuit, this, &ApplicationQt::stop );
	}
	ApplicationQt::~ApplicationQt() {}

	void ApplicationQt::init() { Core::BaseUIApplication::init(); }
	void ApplicationQt::start( const std::vector<std::string> & p_args )
	{
		Core::BaseUIApplication::start( p_args );

		_handleArgs( p_args );

		VTX_EVENT( VTX::App::Event::Global::UI_APPLICATION_INITIALIZED );

		_returnCode = exec();
	}

	void ApplicationQt::update()
	{
		float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
		_elapsedTimer.restart();

		Core::BaseUIApplication::update();
		_stateMachine->update( elapsed );
	}

	void ApplicationQt::quit() { QApplication::quit(); };

	void ApplicationQt::_initUI( const std::vector<std::string> & p_args )
	{
		// Create statemachine.
		_stateMachine = new State::StateMachine();
		goToState( ID::State::VISUALIZATION );

		// Create UI.
		_initQt();

		_instantiateMainWindow();

		_mainWindow->getMainMenu().setCurrentTab( 0 );
		//_renderWidget->setFocus();

		// Start timers.
		_timer = new QTimer( this );
		connect( _timer, &QTimer::timeout, this, &ApplicationQt::update );
		_timer->start( 0 );
		_elapsedTimer.start();
		_tickTimer.start();
	}

	void ApplicationQt::_initQt()
	{
		setWindowIcon( QIcon( ":/sprite/logo.png" ) );

		setStyle( QString::fromStdString( Style::DEFAULT_STYLE_FACTORY ) );

		QPalette appPalette = palette();
		Style::applyApplicationPaletteInPalette( appPalette );
		setPalette( appPalette );

#ifdef _DEBUG
		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
#endif
	}

	void ApplicationQt::_instantiateMainWindow()
	{
		_mainWindow = WidgetFactory::get().instantiateWidget<QT::MainWindow>( nullptr, "MainWindow" );
	}

	void ApplicationQt::_postInit( const std::vector<std::string> & p_args )
	{
		Core::BaseUIApplication::_postInit( p_args );

		_mainWindow->show();
		_mainWindow->initWindowLayout();

		if ( !_mainWindow->isOpenGLValid() )
		{
			UI::QT::Tool::Render::Dialog::openGLInitializationFail();
			return;
		}
	}

	void ApplicationQt::renderScene() const
	{
		if ( VTX_SETTING().getActivateRenderer() && App::VTXApp::get().MASK )
		{
			_mainWindow->updateRender();
		}
	}

	void ApplicationQt::stop()
	{
		_mainWindow->saveLayout();

		// _timer can be uninitialized if critical error append during start (i.e. OpenGL init fail)
		if ( _timer != nullptr )
		{
			_timer->stop();
			delete _timer;
		}

		if ( _stateMachine != nullptr )
		{
			delete _stateMachine;
		}

		// TODO BaseUIApplication::stop() called here because some model are strongly linked to _gl
		BaseUIApplication::stop();

		if ( _mainWindow != nullptr )
		{
			delete _mainWindow;
		}
	}

	void ApplicationQt::goToState( const std::string & p_name, void * const p_arg )
	{
		try
		{
			_stateMachine->goToState( p_name, p_arg );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( p_e.what() );
		}
	}

	bool ApplicationQt::notify( QObject * const receiver, QEvent * const event )
	{
		try
		{
			return QApplication::notify( receiver, event );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( p_e.what() );
#ifdef VTX_PRODUCTION
			UI::Dialog::unhandledException();
#endif
			return true;
		}
	}

} // namespace VTX::UI::QT
