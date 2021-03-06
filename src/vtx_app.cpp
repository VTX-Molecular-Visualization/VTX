#include "vtx_app.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/gl/program_manager.hpp"
#include "selection/selection_manager.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "worker/worker_manager.hpp"
#include <QPalette>
#include <exception>

namespace VTX
{
	int ZERO = 0;
	VTXApp::VTXApp() : QApplication( ZERO, nullptr ) {}

	VTXApp::~VTXApp() {}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application: " + Util::Filesystem::EXECUTABLE_FILE.string() );

		// Create scene.
		_scene = new Object3D::Scene();
		_scene->getCamera().setScreenSize( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		// Create statemachine.
		_stateMachine = new State::StateMachine();
		_stateMachine->goToState( ID::State::VISUALIZATION );

		// Create UI.
		_initQt();
		_mainWindow = new UI::MainWindow();
		_mainWindow->setupUi();
		_mainWindow->show();

		// Create singletons.
		MVC::MvcManager::get();
		Action::ActionManager::get();
		Event::EventManager::get();
		Selection::SelectionManager::get();
		Worker::WorkerManager::get();

		// Load settings.
		VTX_ACTION( new Action::Setting::Load() );

		VTX_INFO( "Application started" );

		// Start timers.
		_timer = new QTimer( this );
		connect( _timer, &QTimer::timeout, this, &VTXApp::_update );
		_timer->start( 0 );
		_elapsedTimer.start();
		_tickTimer.start();

		VTX_ACTION( new Action::Main::Open( Util::Filesystem::getDataPathPtr( "4hhb.pdb" ) ) );
		// VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );

//#define RT_ENABLED
#ifdef RT_ENABLED
		// Path * path	   = new Path( DATA_DIR + "spike_closed_glycans_lipids_amarolab.pdb" );
		FilePath * path = new FilePath( DATA_DIR + "6vsb.mmtf" );
		VTX_ACTION( new Action::Open( path ) );
		VTX_ACTION( new Action::Snapshot( Worker::Snapshoter::MODE::RT ) );
		Action::ActionManager::get().update( 0.f );
		Worker::WorkerManager::get().update( 0.f );
#endif
	}

	void VTXApp::stop()
	{
		_timer->stop();

		delete _timer;

		if ( _stateMachine != nullptr )
		{
			delete _stateMachine;
		}
		if ( _scene != nullptr )
		{
			delete _scene;
		}
		if ( _mainWindow != nullptr )
		{
			delete _mainWindow;
		}

		exit();
	}

	void VTXApp::_initQt()
	{
		this->setWindowIcon( QIcon( ":/sprite/logo.png" ) );

		QPalette appPalette = palette();
		Style::applyApplicationPaletteInPalette( appPalette );
		setPalette( appPalette );

#ifdef _DEBUG
		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
#endif
	}

	void VTXApp::goToState( const std::string & p_name, void * const p_arg )
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

	void VTXApp::_update()
	{
		// Elapsed time.
		float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
		_elapsedTimer.restart();

		// State machine.
		_stateMachine->update( elapsed );

		// Event manager.
		Event::EventManager::get().update( elapsed );

		// Action manager.
		Action::ActionManager::get().update( elapsed );

		// Worker manager.
		Worker::WorkerManager::get().update( elapsed );

		// Tickrate.
		_tickCounter++;
		if ( _tickTimer.elapsed() >= 1000 )
		{
			VTX_STAT().tickRate = _tickCounter / ( _tickTimer.elapsed() * 1e-3 );
			// VTX_INFO( "FPS: " + std::to_string( VTX_STAT().FPS ) + " - "
			//		  + "Tickrate: " + std::to_string( VTX_STAT().tickRate ) + " - "
			//		  + "Render time: " + std::to_string( VTX_STAT().renderTime ) + " ms" );
			_tickCounter = 0;
			_tickTimer.restart();
		}
	}

	void VTXApp::renderScene() const
	{
		if ( VTX_SETTING().activeRenderer && MASK )
		{
			_mainWindow->getOpenGLWidget().update();
		}
	}

	bool VTXApp::notify( QObject * const receiver, QEvent * const event )
	{
		try
		{
			return QApplication::notify( receiver, event );
		}
		catch ( const std::exception & exception )
		{
			VTX_ERROR( exception.what() );
			// throw( exception );
			return true;
		}
	}

} // namespace VTX
