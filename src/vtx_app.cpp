#include "vtx_app.hpp"
#include "id.hpp"
#include "io/path.hpp"
#include "io/reader/reader_mmtf.hpp"
#include "model/model_molecule.hpp"
#include "renderer/renderer_gl.hpp"
#include "ui/imgui/imgui_impl_sdl.h"
#include <thread>

namespace VTX
{
	bool VTXApp::_isRunning;

	VTXApp::VTXApp()
	{
		VTX_INFO( "Initializing application" );
		_ui = new UI::UserInterface();
		_ui->init();
		_stateMachine = new State::Machine();
		_stateMachine->init();
		_scene		   = new Object3D::Scene();
		_renderer	   = new Renderer::RendererGL();
		_actionManager = new Action::ActionManager();
	}

	VTXApp::~VTXApp()
	{
		delete _renderer;
		delete _scene;
		delete _stateMachine;
		delete _actionManager;
		delete _ui;
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application" );
		VTXApp::_isRunning = true;
		VTX_INFO( "Application started" );

		_ui->print();
		_ui->draw();

#ifdef _DEBUG
		//_stateMachine->goToState( ID::State::::VISUALIZATION );
		_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "173D.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "4v6x.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "3j3q.mmtf" ) );
		//_stateMachine->goToState( ID::State::LOADING, &std::string( DATA_DIR + "r2d2.obj" ) );
#else
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

		for ( std::thread * thread : _threads )
		{
			thread->join();
			delete thread;
		}
		_threads.clear();

		VTX_INFO( "Application stopped" );
		VTXApp::_isRunning = false;
	}

	void VTXApp::goToState( const std::string & p_name, void * const p_arg )
	{
		_stateMachine->goToState( p_name, p_arg );
	}

	void VTXApp::fireUIEvent( const Event::EVENT_UI p_event, void * const p_arg ) const
	{
		_ui->receiveEvent( p_event, p_arg );
	}

	void VTXApp::action( Action::BaseAction * const p_action ) const { _actionManager->executeAction( p_action ); }
	void VTXApp::action( const std::string & p_action ) const { _actionManager->executeAction( p_action ); }
	bool VTXApp::canUndo() const { return _actionManager->canUndo(); }
	void VTXApp::undo() const { return _actionManager->undo(); }
	bool VTXApp::canRedo() const { return _actionManager->canRedo(); }
	void VTXApp::redo() const { return _actionManager->redo(); }

	void VTXApp::addThread( std::thread * const p_thread ) { _threads.emplace_back( p_thread ); }

	void VTXApp::setTheme() const { _ui->setTheme(); }

	void VTXApp::initRenderer() const
	{
		// if ( !VTXApp::_isRunning ) { _ui->display(); } // Drawn UI a first time to update display size.
		ImGuiIO & io = ImGui::GetIO();
		_renderer->init( *_scene, (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
	}

	void VTXApp::setRendererShading() const { _renderer->setShading(); }

	void VTXApp::_update()
	{
		// Renderer.
		_chrono.start();
		ImGuiIO & io = ImGui::GetIO();
		_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		_renderer->setSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		_renderer->render( *_scene );

		// UI/machine/events.
		SDL_Event event;
		while ( _ui->pollEvent( event ) )
		{
			_handleEvent( event );
		}
		_ui->draw();
		_scene->update( _timeDelta );
		_stateMachine->update( _timeDelta );

		// Timers.
		_chrono.stop();
		_timeDelta = _chrono.elapsedTime();
		_timeTotal += _timeDelta;
	}

	void VTXApp::_handleEvent( const SDL_Event & p_event )
	{
		// Quit event.
		ImGui_ImplSDL2_ProcessEvent( &p_event );
		switch ( p_event.type )
		{
		case SDL_QUIT: stop(); return;
		case SDL_WINDOWEVENT:
			if ( p_event.window.event == SDL_WINDOWEVENT_CLOSE )
			{
				stop();
				return;
			}
		}

		// Propagate to state machine.
		if ( ImGui::IsAnyWindowFocused() == false ) { _stateMachine->handleEvent( p_event ); }
	}
} // namespace VTX
