#include "vtx_app.hpp"
#include "action/open.hpp"
#include "action/snapshot.hpp"
#include "id.hpp"
#include "io/path.hpp"
#include "model/molecule.hpp"
#include "renderer/gl/gl.hpp"

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

		_scene = new Object3D::Scene();

		_ui->draw(); // Draw the first frame to update screen size.
		ImGuiIO & io = ImGui::GetIO();
		_scene->getCamera().setScreenSize( (int)io.DisplaySize.x, (int)io.DisplaySize.y );

		_renderer = new Renderer::GL();
		_renderer->init( (int)io.DisplaySize.x, (int)io.DisplaySize.y );

		_stateMachine = Generic::create<State::StateMachine>();
		_stateMachine->goToState( ID::State::VISUALIZATION );

		VTXApp::_isRunning = true;

		VTX_INFO( "Application started" );
		_ui->print();

#define AUTO_OPEN
#ifdef AUTO_OPEN
		// VTX_ACTION( new Action::Open( new IO::Path( DATA_DIR + "r2d2_2.obj" ) ) );
		// VTX_ACTION( new Action::Open( new IO::Path( DATA_DIR + "4v6x.mmtf" ) ) );
		VTX_ACTION( new Action::Open( new IO::Path( DATA_DIR + "4jjt.pdb" ) ) );
#endif

//#define RT_ENABLED
#ifdef RT_ENABLED
		// IO::Path * path	   = new IO::Path( DATA_DIR + "spike_closed_glycans_lipids_amarolab.pdb" );
		IO::Path * path = new IO::Path( DATA_DIR + "6vsb.mmtf" );
		VTX_ACTION( new Action::Open( path ) );
		_actionManager->update( 0.f );
		_workerManager->update( 0.f );
		VTX_ACTION( new Action::Snapshot( Worker::Snapshoter::MODE::RT ) );
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
