#include "vtx_app.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
#include "selection/selection_manager.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#ifdef OPTIX_DEFINED
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#endif

namespace VTX
{
	// TODO: ugly...
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

		this->setWindowIcon(QIcon(":/sprite/logo.png"));
		_mainWindow = new UI::MainWindow();
		_mainWindow->show();

		_scene = new Object3D::Scene();
		_scene->getCamera().setScreenSize( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		switchRenderer( Setting::MODE_DEFAULT );

		_stateMachine = new State::StateMachine();
		_stateMachine->goToState( ID::State::VISUALIZATION );

		VTX_ACTION( new Action::Setting::Load() );

		VTX_INFO( "Application started" );

		_timer = new QTimer( this );
		connect( _timer, &QTimer::timeout, this, &VTXApp::_update );
		_timer->start( 0 );

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
		case Renderer::MODE::RT_CPU:
			if ( _rendererRT == nullptr )
			{
				_rendererRT = new Renderer::RayTracer();
				needInit	= true;
			}
			_renderer = _rendererRT;
			break;
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
			else if ( _renderer->getWidth() != getScene().getCamera().getScreenWidth() || _renderer->getHeight() != getScene().getCamera().getScreenHeight() )
			{
				_renderer->resize( getScene().getCamera().getScreenWidth(), getScene().getCamera().getScreenHeight() );
			}
		}
	}

	void VTXApp::_update()
	{
		// TODO: QElapsedTimer?
		double deltaTime = 0.0;

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
