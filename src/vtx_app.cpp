#include "vtx_app.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
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
		_timer		  = new QTimer( this );
		_elapsedTimer = new QElapsedTimer();

		connect( _timer, &QTimer::timeout, this, &VTXApp::_update );

		_timer->start( 0 );
		_elapsedTimer->start();

		VTX_ACTION( new Action::Main::Open( Util::Filesystem::getDataPathPtr( "4hhb.pdb" ) ) );
		// VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );

//#define RT_ENABLED
#ifdef RT_ENABLED
		// Path * path	   = new Path( DATA_DIR + "spike_closed_glycans_lipids_amarolab.pdb" );
		Path * path = new Path( DATA_DIR + "6vsb.mmtf" );
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
		delete _elapsedTimer;

		// Respect this order!
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
		// TODO: check if QTimer and QElapsedTimer can be fused.
		double deltaTime = _elapsedTimer->elapsed() / 1000.0;
		_elapsedTimer->restart();

		// State machine.
		_stateMachine->update( deltaTime );

		// Event manager.
		Event::EventManager::get().update( deltaTime );

		// Action manager.
		Action::ActionManager::get().update( deltaTime );

		// Worker manager.
		Worker::WorkerManager::get().update( deltaTime );
	}

	bool VTXApp::notify( QObject * receiver, QEvent * event )
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
