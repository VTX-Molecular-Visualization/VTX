#include "vtx_app.hpp"
#include "id.hpp"
#include "io/path.hpp"
#include "model/molecule.hpp"
#include "renderer/gl.hpp"
#include <thread>

namespace VTX
{
	bool VTXApp::_isRunning;

	VTXApp::VTXApp()
	{
		VTX_INFO( "Initializing application" );
		_eventManager	  = new Event::EventManager();
		_actionManager	  = new Action::ActionManager();
		_workerManager	  = new Worker::WorkerManager();
		_selectionManager = new Selection::SelectionManager();
	}

	VTXApp::~VTXApp()
	{
		VTX_INFO( "Destructing application" );

		// Respect this order!
		Generic::destroy( _stateMachine );
		Generic::destroy( _scene );
		Generic::destroy( _ui );

		delete _renderer;
		delete _selectionManager;
		delete _workerManager;
		delete _actionManager;
		delete _eventManager;

		VTX_INFO( "Application destructed" );
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application" );

		_ui = Generic::create<UI::UserInterface>();
		_ui->draw(); // Draw the first frame to update screen size.
		ImGuiIO & io = ImGui::GetIO();

		_stateMachine = Generic::create<State::StateMachine>();

		_scene = new Object3D::Scene();
		_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );

		_renderer = new Renderer::GL();
		_renderer->init( (int)io.DisplaySize.x, (int)io.DisplaySize.y );

		VTXApp::_isRunning = true;

		VTX_INFO( "Application started" );

		_ui->print();
		_ui->draw();

#ifdef _DEBUG
		//_stateMachine->goToState( ID::State::VISUALIZATION );
		//_stateMachine->goToState( ID::State::LOAD, &IO::Path( DATA_DIR + "173D.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOAD, &IO::Path( DATA_DIR + "4v6x.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOAD, &IO::Path( DATA_DIR + "6LU7.mmtf" ) );
		//->goToState( ID::State::LOAD, &IO::Path( DATA_DIR + "6LU7.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOADING, &IO::Path( DATA_DIR + "3j3q.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOADING, &IO::Path( DATA_DIR + "r2d2.obj" ) );
		_stateMachine->goToState( ID::State::LOAD, &IO::Path( DATA_DIR + "dhfr2.arc" ) );
#else
		//_stateMachine->goToState( ID::State::LOADING, &IO::Path( DATA_DIR + "4v6x.mmtf" ) );
		_stateMachine->goToState( ID::State::VISUALIZATION );
#endif

		while ( VTXApp::_isRunning )
		{
			_update();
		}
	}

	void VTXApp::stop()
	{
		VTX_INFO( "Stopping application" );

		VTXApp::_isRunning = false;

		VTX_INFO( "Application stopped" );
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
			stop();
		}
	}

	void VTXApp::_update()
	{
		// Set size.
		ImGuiIO & io = ImGui::GetIO();

		// Event manager.
		_eventManager->update( io.DeltaTime );

		// Action manager.
		_actionManager->update( io.DeltaTime );

		// Worker manager.
		_workerManager->update( io.DeltaTime );

		// State machine.
		_stateMachine->update( io.DeltaTime );

		// UI.
		_ui->draw();

		// Timers.
		_timeDelta = io.DeltaTime;
		_timeTotal += _timeDelta;
	}

} // namespace VTX
