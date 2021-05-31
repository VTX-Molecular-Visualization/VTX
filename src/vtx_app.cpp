#include "vtx_app.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/gl/program_manager.hpp"
#include "selection/selection_manager.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "worker/worker_manager.hpp"
#include <QPalette>
#include <QtPlatformHeaders/QWindowsWindowFunctions>
#include <exception>

namespace VTX
{
	int ZERO = 0;
	VTXApp::VTXApp() : QApplication( ZERO, nullptr ) {}

	VTXApp::~VTXApp() {}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application: " + Util::Filesystem::EXECUTABLE_FILE.string() );

		// Load settings.
		VTX_ACTION( new Action::Setting::Load() );
		_setting.loadRecentPaths();

		// Create singletons.
		MVC::MvcManager::get();
		Action::ActionManager::get();
		Event::EventManager::get();
		Selection::SelectionManager::get();
		Worker::WorkerManager::get();

		// Create Databases
		_representationLibrary
			= MVC::MvcManager::get().instantiateModel<Model::Representation::RepresentationLibrary>();
		_renderEffectLibrary = MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPresetLibrary>();
		_renderEffectLibrary->applyPreset( _setting.getDefaultRenderEffectPresetIndex() );

		// Create scene.
		_scene = new Object3D::Scene();
		_scene->getCamera().setScreenSize( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		// Create statemachine.
		_stateMachine = new State::StateMachine();
		_stateMachine->goToState( ID::State::VISUALIZATION );

		// Create UI.
		_initQt();
		_mainWindow = new UI::MainWindow();
		_mainWindow->setupUi();
		_mainWindow->show();

		// Fix Issue for fullscreen on windows. Need to be called after show and before set fullscreen //////////
		// https://doc.qt.io/qt-5/windows-issues.html#fullscreen-opengl-based-windows ///////////////////////////
		QWindowsWindowFunctions::setHasBorderInFullScreen( _mainWindow->windowHandle(), true );
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		_mainWindow->initWindowLayout();
		VTX_INFO( "Application started" );

		// Start timers.
		_timer = new QTimer( this );
		connect( _timer, &QTimer::timeout, this, &VTXApp::_update );
		_timer->start( 0 );
		_elapsedTimer.start();
		_tickTimer.start();

		// VTX_ACTION( new Action::Main::Open( Util::Filesystem::getDataPath( "4hhb.pdb" ) ) );
		VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );

//#define RT_ENABLED
#ifdef RT_ENABLED
		// Path * path	   = new Path( DATA_DIR + "spike_closed_glycans_lipids_amarolab.pdb" );
		FilePath path = FilePath( DATA_DIR + "6vsb.mmtf" );
		VTX_ACTION( new Action::Open( path ) );
		VTX_ACTION( new Action::Snapshot( Worker::Snapshoter::MODE::RT ) );
		Action::ActionManager::get().update( 0.f );
		Worker::WorkerManager::get().update( 0.f );
#endif
	}

	void VTXApp::stop()
	{
		_timer->stop();

		delete _timer;

		// Prevent events throw for nothing when quitting app
		Event::EventManager::get().freezeEvent( true );

		_setting.backup();

		MVC::MvcManager::get().deleteModel( _representationLibrary );
		MVC::MvcManager::get().deleteModel( _renderEffectLibrary );

		Selection::SelectionManager::get().deleteModel();

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

		Setting::cleanRecentPaths();

		exit();
	}

	void VTXApp::_initQt()
	{
		this->setWindowIcon( QIcon( ":/sprite/logo.png" ) );

		QPalette appPalette = palette();
		Style::applyApplicationPaletteInPalette( appPalette );
		setPalette( appPalette );

#ifdef _DEBUG
		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
#endif
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
		// Elapsed time.
		float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
		_elapsedTimer.restart();

		// State machine.
		_stateMachine->update( elapsed );

		// Event manager.
		Event::EventManager::get().update( elapsed );

		// Action manager.
		Action::ActionManager::get().update( elapsed );

		// Worker manager.
		// Worker::WorkerManager::get().update( elapsed );

		// Call late update event for processes at end of frame
		VTX_EVENT( new Event::VTXEvent( Event::Global::LATE_UPDATE ) );

		// Tickrate.
		_tickCounter++;
		if ( _tickTimer.elapsed() >= 1000 )
		{
			VTX_STAT().tickRate = _tickCounter / ( _tickTimer.elapsed() * 1e-3 );
			// VTX_INFO( "FPS: " + std::to_string( VTX_STAT().FPS ) + " - "
			//		  + "Tickrate: " + std::to_string( VTX_STAT().tickRate ) + " - "
			//		  + "Render time: " + std::to_string( VTX_STAT().renderTime ) + " ms" );
			_tickCounter = 0;
			_tickTimer.restart();
		}
	}

	void VTXApp::renderScene() const
	{
		if ( VTX_SETTING().getActivateRenderer() && MASK )
		{
			_mainWindow->getOpenGLWidget().update();
		}
	}

	Model::Renderer::RenderEffectPreset & VTX_RENDER_EFFECT()
	{
		return Model::Renderer::RenderEffectPresetLibrary::get().getAppliedPreset();
	}

	bool VTXApp::notify( QObject * const receiver, QEvent * const event )
	{
		try
		{
			return QApplication::notify( receiver, event );
		}
		catch ( const std::exception & exception )
		{
			VTX_ERROR( exception.what() );
			// throw( exception );
			return true;
		}
	}

} // namespace VTX
