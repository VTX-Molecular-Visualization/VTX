#include "vtx_app.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "id.hpp"
#include "model/molecule.hpp"
#include "renderer/gl/gl.hpp"
#include "util/filesystem.hpp"

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
		if ( _stateMachine != nullptr )
		{
			Generic::destroy( _stateMachine );
		}
		if ( _scene != nullptr )
		{
			Generic::destroy( _scene );
		}
		if ( _ui != nullptr )
		{
			Generic::destroy( _ui );
		}
		if ( _rendererGL != nullptr )
		{
			delete _rendererGL;
		}
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

		_ui = Generic::create<UI::UserInterface>();

		_scene = new Object3D::Scene();

		_scene->getCamera().setScreenSize( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		_rendererGL = new Renderer::GL();
		_rendererGL->init( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		_stateMachine = Generic::create<State::StateMachine>();
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
		VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );
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

	void to_json( nlohmann::json & p_json, const VTXApp & p_app )
	{
		// p_json = json { { "SCENE", p_app.getScene() } };
		VTX_DEBUG( "TOJSON" );
	}

	void from_json( const nlohmann::json & p_json, VTXApp & p_app )
	{
		// p_json.at( "SCENE" ).get_to( p_app.getScene() );
		VTX_DEBUG( "FROMJSON" );
	}
} // namespace VTX
