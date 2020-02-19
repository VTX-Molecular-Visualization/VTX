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
		_eventManager  = new Event::EventManager();
		_actionManager = new Action::ActionManager();
		_scene		   = new Object3D::Scene();
		_renderer	   = new Renderer::GL();
	}

	VTXApp::~VTXApp()
	{
		// Respect this order!
		Generic::destroy( _stateMachine );
		Generic::destroy( _scene );
		Generic::destroy( _ui );

		delete _renderer;
		delete _actionManager;
		delete _eventManager;
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application" );

		_stateMachine	   = Generic::create<State::StateMachine>();
		_ui				   = Generic::create<UI::UserInterface>();
		VTXApp::_isRunning = true;

		VTX_INFO( "Application started" );

		_ui->print();
		_ui->draw();

#ifdef _DEBUG
		//_stateMachine->goToState( ID::State::VISUALIZATION );
		//_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "173D.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOAD, &std::string( DATA_DIR + "4v6x.mmtf" ) );
		_stateMachine->goToState( ID::State::LOAD, &std::string( DATA_DIR + "6LU7.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "3j3q.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "r2d2.obj" ) );
#else
		//_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "4v6x.mmtf" ) );
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

	void VTXApp::action( Action::BaseAction * const p_action ) const { _actionManager->executeAction( p_action ); }
	void VTXApp::action( const std::string & p_action ) const { _actionManager->executeAction( p_action ); }
	bool VTXApp::canUndo() const { return _actionManager->canUndo(); }
	void VTXApp::undo() const { return _actionManager->undo(); }
	bool VTXApp::canRedo() const { return _actionManager->canRedo(); }
	void VTXApp::redo() const { return _actionManager->redo(); }

	void VTXApp::initRenderer() const
	{
		// if ( !VTXApp::_isRunning ) { _ui->draw(); } // Drawn UI a first time to update display size.
		ImGuiIO & io = ImGui::GetIO();
		_renderer->init( *_scene, (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
	}

	void VTXApp::_update()
	{
		_chrono.start();

		// Set size.
		// TODO: resize.
		// ImGuiIO & io = ImGui::GetIO();
		//_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		//_renderer->setSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );

		// Event manager.
		_eventManager->update( _timeDelta );

		// State machine.
		_stateMachine->update( _timeDelta );

		// UI.
		_ui->draw();

		// Timers.
		_chrono.stop();
		_timeDelta = _chrono.elapsedTime();
		_timeTotal += _timeDelta;
	}

} // namespace VTX
