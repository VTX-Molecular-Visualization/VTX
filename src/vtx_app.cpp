#include "vtx_app.hpp"
#include "io/path.hpp"
#include "io/reader/reader_mmtf.hpp"
#include "model/model_molecule.hpp"
#include "renderer/renderer_deferred.hpp"
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
		_ui->printInfos();

		_stateMachine->goToState( State::STATE_NAME::LOADING );

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

	void VTXApp::runThread( std::thread * const p_thread ) { _threads.emplace_back( p_thread ); }

	void VTXApp::setTheme() const { _ui->setTheme(); }

	void VTXApp::initRenderer() const
	{
		// TODO: throw exceptionn if renderer null.
		_renderer->init( _scene, View::BaseView3DMolecule::REPRESENTATION::BALL_AND_STICK );
	}

	void VTXApp::_update()
	{
		// UI.
		_chrono.start();
		_ui->display();
		_chrono.stop();
		_timeLastUI = _chrono.elapsedTime();

		// State machine.
		_stateMachine->update();

		// Renderer.
		_chrono.start();
		_renderer->render( _scene );
		_chrono.stop();
		_timeLastRenderer = _chrono.elapsedTime();

		// Timers.
		_timeLast = _timeLastUI;
		_timeLast += _timeLastRenderer;
		_timeTotal += _timeLast;
	}
} // namespace VTX
