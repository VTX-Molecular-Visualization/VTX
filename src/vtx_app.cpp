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
		VTXApp::_isRunning = true;
		VTX_INFO( "Application started" );

		_ui->display();
		_ui->printInfos();

#ifdef _DEBUG
		_stateMachine->goToState( State::STATE_NAME::LOADING, &std::string( DATA_DIR + "r2d2_2.obj" ) );
#else
		_stateMachine->goToState( State::STATE_NAME::VISUALIZATION );
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

	const std::shared_ptr<UI::BaseComponent> VTXApp::getUIComponentByName( const UI::COMPONENT_NAME p_name ) const
	{
		return _ui->getComponentByName( p_name );
	};

	void VTXApp::goToState( const State::STATE_NAME p_name, void * const p_arg )
	{
		_stateMachine->goToState( p_name, p_arg );
	}

	void VTXApp::fireUIEvent( const Event::EVENT_UI p_event, void * const p_arg ) const
	{
		_ui->receiveEvent( p_event, p_arg );
	}

	void VTXApp::addThread( std::thread * const p_thread ) { _threads.emplace_back( p_thread ); }

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
		_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		_renderer->setSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );
		_renderer->render( *_scene );

		// UI/machine/events.
		SDL_Event event;
		while ( _ui->pollEvent( event ) )
		{
			_handleEvent( event );
		}
		_ui->display();
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
			if ( p_event.window.event == SDL_WINDOWEVENT_CLOSE ) stop();
			return;
		}

		// Propagate to state machine.
		_stateMachine->handleEvent( p_event );
	}
} // namespace VTX
