#include "vtx_app.hpp"
#include "action/open.hpp"
#include "id.hpp"
#include "io/path.hpp"
#include "model/molecule.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/optix_ray_tracer/optix_ray_tracer.hpp"
#include "renderer/ray_tracing/ray_tracer.hpp"
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

		_scene = new Object3D::Scene();
		_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );

		_renderer = new Renderer::GL();
		_renderer->init( (int)io.DisplaySize.x, (int)io.DisplaySize.y );

		_stateMachine = Generic::create<State::StateMachine>();
		_stateMachine->goToState( ID::State::VISUALIZATION );

		VTXApp::_isRunning = true;

		VTX_INFO( "Application started" );
		_ui->print();

#ifdef _DEBUG
		// IO::Path path = DATA_DIR + "6vsb.mmtf";
		VTX_ACTION( new Action::Open( new IO::Path( DATA_DIR + "6vsb.mmtf" ) ) );
		// VTX_ACTION( new Action::Open( DATA_DIR + "4v6x.mmtf" ) );
		// VTX_ACTION( new Action::Open(DATA_DIR + "6LU7.mmtf" ) );
		// VTX_ACTION( new Action::Open(DATA_DIR + "6LU7.mmtf" ) );
		// VTX_ACTION( new Action::Open(DATA_DIR + "3j3q.mmtf" ) );
		// VTX_ACTION( new Action::Open(DATA_DIR + "r2d2.obj" ) );
		// VTX_ACTION( new Action::Open( DATA_DIR + "dhfr2.arc" ) );
		// VTX_ACTION( new Action::Open( DATA_DIR + "6lzg.pdb" ) );
#else
		IO::Path path = DATA_DIR + "6vsb.mmtf";
		VTX_ACTION( new Action::Open( &path ) );
		// VTX_ACTION( new Action::Open( DATA_DIR + "dhfr2.arc" ) );
#endif

#define RT_ENABLED
#ifdef RT_ENABLED
		VTXApp::_isRunning = false;

		_actionManager->update( 0.f );
		_workerManager->update( 0.f );

//#define OPTIX_DEFINED
#ifdef OPTIX_DEFINED
		Renderer::OptixRayTracer * ort = new Renderer::OptixRayTracer();
		ort->init( int( io.DisplaySize.x ), int( io.DisplaySize.y ) );
		ort->renderFrame( *_scene );
		delete ort;
#else
		Renderer::RayTracer * rt = new Renderer::RayTracer();
		rt->init( int( io.DisplaySize.x ), int( io.DisplaySize.y ) );
		rt->renderFrame( *_scene );
		delete rt;
#endif
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
		float deltaTIme = ImGui::GetIO().DeltaTime;

		// State machine.
		_stateMachine->update( deltaTIme );

		// Event manager.
		_eventManager->update( deltaTIme );

		// Action manager.
		_actionManager->update( deltaTIme );

		// Worker manager.
		_workerManager->update( deltaTIme );

		// UI.
		_ui->draw();
	}

} // namespace VTX
