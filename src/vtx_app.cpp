#include "vtx_app.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "renderer/gl/gl.hpp"
#include "selection/selection_manager.hpp"
#include "util/filesystem.hpp"

namespace VTX
{
	bool VTXApp::_isRunning;

	VTXApp::VTXApp()
	{
		_eventManager	  = new Event::EventManager();
		_actionManager	  = new Action::ActionManager();
		_workerManager	  = new Worker::WorkerManager();
		_selectionManager = new Selection::SelectionManager();
	}

	VTXApp::~VTXApp()
	{
		VTX_INFO( "Destructing application" );

		// Respect this order!
		if ( _stateMachine != nullptr )
		{
			delete _stateMachine;
		}
		if ( _scene != nullptr )
		{
			delete _scene;
		}
		if ( _ui != nullptr )
		{
			delete _ui;
		}
		if ( _rendererGL != nullptr )
		{
			delete _rendererGL;
		}
#ifdef CUDA_DEFINED
		if ( _rendererRT != nullptr )
		{
			delete _rendererRT;
		}
#endif
#ifdef OPTIX_DEFINED
		if ( _rendererOptix != nullptr )
		{
			delete _rendererOptix;
		}
#endif
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

	void VTXApp::start()
	{
		VTX_INFO( "Starting application: " + Util::Filesystem::EXECUTABLE_FILE.string() );

		_ui = new UI::UserInterface();

		_scene = new Object3D::Scene();

		_scene->getCamera().setScreenSize( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		switchRenderer( Setting::MODE_DEFAULT );

		_stateMachine = new State::StateMachine();
		_stateMachine->goToState( ID::State::VISUALIZATION );

		VTX_ACTION( new Action::Setting::Load() );

		VTXApp::_isRunning = true;

		VTX_INFO( "Application started" );
		_ui->print();

#define AUTO_OPEN
#ifdef AUTO_OPEN
		// VTX_ACTION( new Action::Open( Util::Filesystem::getDataPathPtr( "r2d2_2.obj" ) ) );
		// VTX_ACTION( new Action::Open( Util::Filesystem::getDataPathPtr( "4v6x.mmtf" ) ) );
		// VTX_ACTION( new Action::Open( Util::Filesystem::getDataPathPtr("6vsb.mmtf" ) ) );
		// VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );
		// VTX_ACTION( new Action::Open( Util::Filesystem::getDataPathPtr( "3jb9.pdb" ) ) );
#endif

//#define RT_ENABLED
#ifdef RT_ENABLED
		// Path * path	   = new Path( DATA_DIR + "spike_closed_glycans_lipids_amarolab.pdb" );
		Path * path = new Path( DATA_DIR + "6vsb.mmtf" );
		VTX_ACTION( new Action::Open( path ) );
		VTX_ACTION( new Action::Snapshot( Worker::Snapshoter::MODE::RT ) );
		_actionManager->update( 0.f );
		_workerManager->update( 0.f );
		_isRunning = false;
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
		}
	}

	void VTXApp::switchRenderer( const Renderer::MODE p_mode )
	{
		bool needInit = false;

		switch ( p_mode )
		{
		case Renderer::MODE::GL:
			if ( _rendererGL == nullptr )
			{
				_rendererGL = new Renderer::GL();
				needInit	= true;
			}
			_renderer = _rendererGL;
			break;
#ifdef CUDA_DEFINED
		case Renderer::MODE::RT_CPU:
			if ( _rendererRT == nullptr )
			{
				_rendererRT = new Renderer::RayTracer();
				needInit	= true;
			}
			_renderer = _rendererRT;
			break;
#endif
#ifdef OPTIX_DEFINED
		case Renderer::MODE::RT_OPTIX:
			if ( _rendererOptix == nullptr )
			{
				_rendererOptix = new Renderer::Optix::OptixRayTracer();
				needInit	   = true;
			}
			_renderer = _rendererOptix;
			break;
#endif

		default: _renderer = nullptr;
		}

		if ( _renderer != nullptr )
		{
			if ( needInit )
			{
				_renderer->init( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );
			}
			// Resize if needed.
			else if ( _renderer->getWidth() != getScene().getCamera().getScreenWidth()
					  || _renderer->getHeight() != getScene().getCamera().getScreenHeight() )
			{
				_renderer->resize( getScene().getCamera().getScreenWidth(), getScene().getCamera().getScreenHeight() );
			}
		}
	}

	void VTXApp::_update()
	{
		const float deltaTime = ImGui::GetIO().DeltaTime;

		// State machine.
		_stateMachine->update( deltaTime );

		// Event manager.
		_eventManager->update( deltaTime );

		// Action manager.
		_actionManager->update( deltaTime );

		// Worker manager.
		_workerManager->update( deltaTime );

		// UI.
		_ui->draw();
	}

} // namespace VTX
