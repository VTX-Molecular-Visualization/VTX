#include "vtx_app.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "io/struct/scene_path_data.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "mvc/mvc_manager.hpp"
#include "renderer/gl/program_manager.hpp"
#include "selection/selection_manager.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "worker/worker_manager.hpp"
#include <QPalette>
#include <QScreen>
#include <exception>

namespace VTX
{
	int ZERO = 0;
	VTXApp::VTXApp() : QApplication( ZERO, nullptr )
	{
		connect( this, &QCoreApplication::aboutToQuit, this, &VTXApp::_stop );
	}

	VTXApp::~VTXApp() {}

	void VTXApp::start( const std::vector<std::string> & p_args )
	{
		VTX_INFO( "Starting application: " + Util::Filesystem::getExecutableFile().path() );

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
		_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );

		// Create scene.
		_scene = new Object3D::Scene();
		_scene->getCamera().setScreenSize( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );

		_pathSceneData = new IO::Struct::ScenePathData();

		// Create statemachine.
		_stateMachine = new State::StateMachine();
		goToState( ID::State::VISUALIZATION );

		// Create UI.
		_initQt();
		_mainWindow = new UI::MainWindow();
		_mainWindow->setupUi();
		_mainWindow->show();

		// Fix Issue for fullscreen on windows. Need to be called after show and before set fullscreen //////////
		// https://doc.qt.io/qt-5/windows-issues.html#fullscreen-opengl-based-windows ///////////////////////////
		// QWindowsWindowFunctions::setHasBorderInFullScreen( _mainWindow->windowHandle(), true );
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		_mainWindow->initWindowLayout();
		VTX_INFO( "Application started" );

		if ( !_mainWindow->isOpenGLValid() )
		{
			UI::Dialog::openGLInitializationFail();
			return;
		}

		if ( VTX_SETTING().getCheckVTXUpdateAtLaunch() )
			VTX_ACTION( new Action::Main::CheckForUpdate() );

		// Start timers.
		_timer = new QTimer( this );
		connect( _timer, &QTimer::timeout, this, &VTXApp::_update );
		_timer->start( 0 );
		_elapsedTimer.start();
		_tickTimer.start();

		_handleArgs( p_args );

#ifndef VTX_PRODUCTION
		if ( p_args.size() == 0 )
		{
			// VTX_ACTION( new Action::Main::Open( Util::Filesystem::getDataPath( IO::FilePath( "4hhb.pdb" ) ) ) );
			VTX_ACTION( new Action::Main::OpenApi( "4hhb" ) );
		}
#endif
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

	void VTXApp::_handleArgs( const std::vector<std::string> & p_args )
	{
		std::vector<IO::FilePath> files	 = std::vector<IO::FilePath>();
		std::vector<std::string>  pdbIds = std::vector<std::string>();

		for ( const std::string & arg : p_args )
		{
			if ( arg.find( "." ) != std::string::npos )
			{
				files.emplace_back( IO::FilePath( arg ) );
			}
			else
			{
				pdbIds.emplace_back( arg );
			}
		}

		VTX_ACTION( new Action::Main::Open( files ) );

		for ( const std::string & pdbId : pdbIds )
		{
			VTX_ACTION( new Action::Main::OpenApi( pdbId ) );
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

	bool VTXApp::hasAnyModifications() const
	{
		const bool hasSavePath	   = !getScenePathData().getCurrentPath().empty();
		const bool sceneIsEmpty	   = getScene().isEmpty();
		const bool sceneHasChanged = getScenePathData().sceneHasModifications();
#ifdef VTX_PRODUCTION
		return ( !hasSavePath && !sceneIsEmpty ) || ( hasSavePath && sceneHasChanged );
#else
		return hasSavePath && sceneHasChanged;
#endif
	}

	void VTXApp::_stop()
	{
		// _timer can be uninitialized if critical error append during start (i.e. OpenGL init fail)
		if ( _timer != nullptr )
		{
			_timer->stop();
			delete _timer;
		}
		// Prevent events throw for nothing when quitting app
		Event::EventManager::get().freezeEvent( true );
		Worker::WorkerManager::get().stopAll();

		_setting.backup();
		_mainWindow->saveLayout();

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

	float VTXApp::getPixelRatio() const
	{
		const QPoint		  globalCenter = _mainWindow->mapToGlobal( _mainWindow->rect().center() );
		const QScreen * const screenPtr	   = VTXApp::get().screenAt( globalCenter );

		return screenPtr == nullptr ? 1.f : float( screenPtr->devicePixelRatio() );
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
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( p_e.what() );
#ifdef VTX_PRODUCTION
			UI::Dialog::unhandledException();
#endif
			return true;
		}
	}

} // namespace VTX
