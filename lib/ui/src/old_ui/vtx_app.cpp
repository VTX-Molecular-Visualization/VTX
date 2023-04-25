#include "ui/old_ui/vtx_app.hpp"
//// #include "ui/action/action_manager.hpp"
//// #include "ui/action/main.hpp"
//// #include "ui/action/setting.hpp"
//// #include <app/core/event/vtx_event.hpp>
//// #include "ui/event/event_manager.hpp"
//// #include "ui/io/struct/scene_path_data.hpp"
//// #include <app/core/mvc/mvc_manager.hpp>
//// #include "ui/renderer/gl/program_manager.hpp"
//// #include "ui/selection/selection_manager.hpp"
//// #include <app/model/renderer/render_effect_preset.hpp"
//// #include <app/model/renderer/render_effect_preset_library.hpp"
//// #include <app/model/representation/representation_library.hpp"
////// #include "ui/old_ui/ui/dialog.hpp"
////// #include "ui/old_ui/ui/main_window.hpp"
//// #include "ui/io/filesystem.hpp"
//// #include "ui/worker/worker_manager.hpp"
//// #include <QApplication>
//// #include <QPalette>
//// #include <exception>
//
// namespace VTX
//{
//	// int ZERO = 0;
//	VTXApp::VTXApp() // : QApplication( ZERO, nullptr )
//	{
//		// connect( this, &QCoreApplication::aboutToQuit, this, &VTXApp::_stop );
//	}
//
//	VTXApp::~VTXApp() {}
//
//	void VTXApp::start( const std::vector<std::string> & p_args )
//	{
//		// VTX_INFO( "Starting application: {}", IO::Filesystem::EXECUTABLE_ABSOLUTE_PATH.string() );
//
//		//		// Load settings.
//		//		VTX_ACTION( new App::Action::Setting::Load() );
//		//		_setting.loadRecentPaths();
//		//
//		//		// Create singletons.
//		//		VTX::Core::MVC::MvcManager::get();
//		//		Action::ActionManager::get();
//		//		Event::EventManager::get();
//		//		Selection::SelectionManager::get();
//		//		Worker::WorkerManager::get();
//		//
//		//		// Create Databases
//		//		_representationLibrary
//		//			= VTX::Core::MVC::MvcManager::get().instantiateModel<Model::Representation::RepresentationLibrary>();
//		//		_renderEffectLibrary =
//		// VTX::Core::MVC::MvcManager::get().instantiateModel<Model::Renderer::RenderEffectPresetLibrary>();
//		//		_renderEffectLibrary->setAppliedPreset( _setting.getDefaultRenderEffectPresetIndex() );
//		//
//		//		// Create scene.
//		//		_scene = new Object3D::Scene();
//		//		_scene->getCamera().setScreenSize( Style::WINDOW_WIDTH_DEFAULT, Style::WINDOW_HEIGHT_DEFAULT );
//		//
//		//		_pathSceneData = new IO::Struct::ScenePathData();
//		//
//		//		if ( VTX_SETTING().getCheckVTXUpdateAtLaunch() )
//		//		{
//		//			VTX_ACTION( new App::Action::Main::CheckForUpdate() );
//		//		}
//		//
//		//		// Start timers.
//		//		//_timer = new QTimer( this );
//		//		// connect( _timer, &QTimer::timeout, this, &VTXApp::_update );
//		//		//_timer->start( 0 );
//		//		//_elapsedTimer.start();
//		//		//_tickTimer.start();
//		//
//		//		_handleArgs( p_args );
//		//
//		// #ifndef VTX_PRODUCTION
//		//		if ( p_args.size() == 0 )
//		//		{
//		//			// VTX_ACTION(
//		//			//	 new App::Action::Main::Open( IO::Filesystem::getDataPath( FilePath( "4hhb.pdb" ) ).absolute() ) );
//		//			// VTX_ACTION( new App::Action::Main::OpenApi( "1aon" ) );
//		//			// VTX_ACTION( new App::Action::Main::OpenApi( "4hhb" ) );
//		//			// VTX_ACTION( new App::Action::Main::OpenApi( "1aga" ) );
//		//		}
//		// #endif
//	}
//	void VTXApp::update() { _update(); }
//	void VTXApp::stop() { _stop(); }
//
//	void VTXApp::_initQt()
//	{
//		//		this->setWindowIcon( QIcon( ":/sprite/logo.png" ) );
//		//
//		//		QPalette appPalette = palette();
//		//		Style::applyApplicationPaletteInPalette( appPalette );
//		//		setPalette( appPalette );
//		//
//		// #ifdef _DEBUG
//		//		QLoggingCategory::setFilterRules( QStringLiteral( "qt.gamepad.debug=true" ) );
//		// #endif
//	}
//
//	void VTXApp::_handleArgs( const std::vector<std::string> & p_args )
//	{
//		// std::vector<FilePath>	 files	= std::vector<FilePath>();
//		// std::vector<std::string> pdbIds = std::vector<std::string>();
//
//		// for ( const std::string & arg : p_args )
//		//{
//		//	if ( arg.find( "." ) != std::string::npos )
//		//	{
//		//		files.emplace_back( FilePath( arg ) );
//		//	}
//		//	else
//		//	{
//		//		pdbIds.emplace_back( arg );
//		//	}
//		// }
//
//		// VTX_ACTION( new App::Action::Main::Open( files ) );
//
//		// for ( const std::string & pdbId : pdbIds )
//		//{
//		//	VTX_ACTION( new App::Action::Main::OpenApi( pdbId ) );
//		// }
//	}
//
//	void VTXApp::_update()
//	{
//		//// Elapsed time.
//		// float elapsed = _elapsedTimer.nsecsElapsed() * 1e-9;
//		//_elapsedTimer.restart();
//
//		//// Useless: nothing is delayed.
//		//// Event manager.
//		//// VTX::App::Core::Event::EventManager::get().update( elapsed );
//
//		//// Action manager.
//		//// Action::ActionManager::get().update( elapsed );
//
//		//_applyEndOfFrameDeletes();
//
//		//// Call late update event for processes at end of frame
//		// VTX_EVENT( new VTX::App::Core::Event::VTXEvent( VTX::App::Event::Global::LATE_UPDATE ) );
//
//		//// Tickrate.
//		//_tickCounter++;
//		// if ( _tickTimer.elapsed() >= 1000 )
//		//{
//		//	VTX_STAT().tickRate = _tickCounter / ( _tickTimer.elapsed() * 1e-3 );
//		//	// VTX_INFO( "FPS: " + std::to_string( VTX_STAT().FPS ) + " - "
//		//	//		  + "Tickrate: " + std::to_string( VTX_STAT().tickRate ) + " - "
//		//	//		  + "Render time: " + std::to_string( VTX_STAT().renderTime ) + " ms" );
//		//	_tickCounter = 0;
//		//	_tickTimer.restart();
//		//}
//	}
//
//	bool VTXApp::hasAnyModifications() const
//	{
//		//		const bool hasSavePath	   = !getScenePathData().getCurrentPath().empty();
//		//		const bool sceneIsEmpty	   = getScene().isEmpty();
//		//		const bool sceneHasChanged = getScenePathData().sceneHasModifications();
//		// #ifdef VTX_PRODUCTION
//		//		return ( !hasSavePath && !sceneIsEmpty ) || ( hasSavePath && sceneHasChanged );
//		// #else
//		//		return hasSavePath && sceneHasChanged;
//		// #endif
//	}
//
//	void VTXApp::_stop()
//	{
//		//// _timer can be uninitialized if critical error append during start (i.e. OpenGL init fail)
//		// if ( _timer != nullptr )
//		//{
//		//	_timer->stop();
//		//	delete _timer;
//		// }
//		//// Prevent events throw for nothing when quitting app
//		// VTX::App::Core::Event::EventManager::get().freezeEvent( true );
//		// Worker::WorkerManager::get().stopAll();
//
//		//_setting.backup();
//
//		// VTX::Core::MVC::MvcManager::get().deleteModel( _representationLibrary );
//		// VTX::Core::MVC::MvcManager::get().deleteModel( _renderEffectLibrary );
//
//		// Selection::SelectionManager::get().deleteModel();
//
//		// if ( _scene != nullptr )
//		//{
//		//	delete _scene;
//		// }
//	}
//
//	void VTXApp::goToState( const std::string & p_name, void * const p_arg ) {}
//
//	void VTXApp::renderScene() const {}
//
//	void VTXApp::deleteAtEndOfFrame( const Generic::BaseAutoDelete * const p_object )
//	{
//		//_deleteAtEndOfFrameObjects.emplace_back( p_object );
//	}
//	void VTXApp::_applyEndOfFrameDeletes()
//	{
//		// for ( const Generic::BaseAutoDelete * const p_object : _deleteAtEndOfFrameObjects )
//		//	p_object->autoDelete();
//
//		//_deleteAtEndOfFrameObjects.clear();
//	}
//
//	void VTXApp::closeAllWindows()
//	{
//		// QApplication::closeAllWindows();
//	}
//	void VTXApp::exit( int p_returnCode )
//	{
//		// QApplication::exit(p_returnCode);
//	}
//	void VTXApp::quit()
//	{
//		// QApplication::quit();
//	}
//
//	// Model::Renderer::RenderEffectPreset & VTX_RENDER_EFFECT()
//	//{
//	//	// return Model::Renderer::RenderEffectPresetLibrary::get().getAppliedPreset();
//	// }
//
//	//	bool VTXApp::notify( QObject * const receiver, QEvent * const event )
//	//	{
//	//		try
//	//		{
//	//			return QApplication::notify( receiver, event );
//	//		}
//	//		catch ( const std::exception & p_e )
//	//		{
//	//			VTX_ERROR( p_e.what() );
//	// #ifdef VTX_PRODUCTION
//	//			UI::Dialog::unhandledException();
//	// #endif
//	//			return true;
//	//		}
//	//	}
//
//} // namespace VTX
