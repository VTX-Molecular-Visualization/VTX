#include "vtx_app.hpp"
#include "io/path.hpp"
#include "io/reader/reader_mmtf.hpp"
#include "model/model_molecule.hpp"
#include "renderer/renderer_deferred.hpp"
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
		_stateMachine = new State::StateMachine();
		_stateMachine->init();
		_scene	  = new Object3D::Scene();
		_renderer = new Renderer::RendererDeferred();
	}

	VTXApp::~VTXApp()
	{
		delete _ui;
		delete _stateMachine;
		delete _scene;
		delete _renderer;
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application" );
		VTX_INFO( "Application started" );
		_ui->printInfos();

		_stateMachine->goToState( State::STATE_NAME::LOADING );
		VTXApp::_isRunning = true;

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

	const std::shared_ptr<UI::BaseComponent> VTXApp::getUIComponentByName( const UI::COMPONENT_NAME p_name ) const
	{
		return _ui->getComponentByName( p_name );
	};

	void VTXApp::goToState( const State::STATE_NAME p_name ) { _stateMachine->goToState( p_name ); }

	void VTXApp::fireUIEvent( const Event::EVENT_UI p_event, void * const p_arg ) const
	{
		_ui->receiveEvent( p_event, p_arg );
	}

	// void VTXApp::addThread( std::thread * const p_thread ) { _threads.emplace_back( p_thread ); }

	void VTXApp::setTheme() const { _ui->setTheme(); }

	void VTXApp::initRenderer() const
	{
		if ( !VTXApp::_isRunning ) _ui->display(); // Drawn UI a first time to update display size.
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
		//_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		//_renderer->setSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		_renderer->render( *_scene );
		_chrono.stop();
		_timeLastRenderer = _chrono.elapsedTime();

		// UI/machine/events.
		_chrono.start();
		SDL_Event event;
		while ( _ui->pollEvent( event ) )
		{
			_handleEvent( event );
		}
		_ui->display();
		_stateMachine->update();
		_chrono.stop();
		_timeLastUI = _chrono.elapsedTime();

		// Timers.
		_timeLast = _timeLastUI;
		_timeLast += _timeLastRenderer;
		_timeTotal += _timeLast;
	}

	void VTXApp::_handleEvent( const SDL_Event & p_event )
	{
		// Quit event.
		ImGui_ImplSDL2_ProcessEvent( &p_event );
		switch ( p_event.type )
		{
		case SDL_QUIT: stop(); return;
		case SDL_WINDOWEVENT:
			if ( p_event.window.event == SDL_WINDOWEVENT_CLOSE ) stop();
			return;
		}

		// Propagate to state machine.
		_stateMachine->handleEvent( p_event );
	}
} // namespace VTX
