#include "vtx_app.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "selection/selection_manager.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"

namespace VTX
{
	int ZERO = 0;
	VTXApp::VTXApp() : QApplication( ZERO, nullptr )
	{
		_eventManager	  = new Event::EventManager();
		_actionManager	  = new Action::ActionManager();
		_workerManager	  = new Worker::WorkerManager();
		_selectionManager = new Selection::SelectionManager();
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application: " + Util::Filesystem::EXECUTABLE_FILE.string() );

		_scene = new Object3D::Scene();
		_scene->getCamera().setScreenSize( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		_stateMachine = new State::StateMachine();
		_stateMachine->goToState( ID::State::VISUALIZATION );

		VTX_ACTION( new Action::Setting::Load() );

		this->setWindowIcon( QIcon( ":/sprite/logo.png" ) );
		_mainWindow = new UI::MainWindow();
		_mainWindow->show();

		VTX_INFO( "Application started" );

		_timer		  = new QTimer( this );
		_elapsedTimer = new QElapsedTimer();

		connect( _timer, &QTimer::timeout, this, &VTXApp::_update );

		_timer->start( 0 );
		_elapsedTimer->start();

		// VTX_ACTION( new Action::Main::Open( Util::Filesystem::getDataPathPtr( "4hhb.pdb" ) ) );
		// VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );

//#define RT_ENABLED
#ifdef RT_ENABLED
		// Path * path	   = new Path( DATA_DIR + "spike_closed_glycans_lipids_amarolab.pdb" );
		Path * path = new Path( DATA_DIR + "6vsb.mmtf" );
		VTX_ACTION( new Action::Open( path ) );
		VTX_ACTION( new Action::Snapshot( Worker::Snapshoter::MODE::RT ) );
		_actionManager->update( 0.f );
		_workerManager->update( 0.f );
#endif
	}

	void VTXApp::stop() { _timer->stop(); }

	VTXApp::~VTXApp()
	{
		VTX_INFO( "Destructing application" );

		delete _timer;

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
		if ( _selectionManager != nullptr )
		{
			delete _selectionManager;
		}
		if ( _workerManager != nullptr )
		{
			delete _workerManager;
		}
		if ( _actionManager != nullptr )
		{
			delete _actionManager;
		}
		if ( _eventManager != nullptr )
		{
			delete _eventManager;
		}
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
		_eventManager->update( deltaTime );

		// Action manager.
		_actionManager->update( deltaTime );

		// Worker manager.
		_workerManager->update( deltaTime );
	}

} // namespace VTX
