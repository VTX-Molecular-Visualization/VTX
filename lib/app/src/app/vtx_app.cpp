#include "app/vtx_app.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/controller.hpp"
#include "app/events.hpp"
#include "app/filesystem.hpp"
#include "app/input/input_manager.hpp"
#include "app/library/library_manager.hpp"
#include "app/library/preset/color_layout.hpp"
#include "app/library/preset/render_settings.hpp"
#include "app/library/preset/representation.hpp"
#include "app/network/network_manager.hpp"
#include "app/pass/camera_updater.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include "app/threading/thread_manager.hpp"
#include "app/uid/uid_manager.hpp"
#include <exception>
#include <renderer/facade.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <util/monitoring/stats.hpp>

namespace
{
	/**
	 * @brief Store local entities.
	 */
	using namespace VTX::App;
	ECS::Entity _scene;
	ECS::Entity _camera;
} // namespace

namespace VTX::App
{

	VTXApp::VTXApp( const Args & p_args )
	{
		// Set global registry.
		ECS::setRegistry( _registry );

		// Store args.
		ECS::setCtx<Args>( p_args );
		// Store main event bus.
		ECS::setCtx<Util::EventHub>();
		// Store statistics.
		ECS::setCtx<Util::Monitoring::Stats>();
		// Store renderer.
		ECS::setCtx<Renderer::Facade>().setDefault();
		// Store action manager.
		ECS::setCtx<Action::ActionManager>();
		// Store input manager.
		ECS::setCtx<Input::InputManager>();
		// Store library manager.
		ECS::setCtx<Library::LibraryManager>();
		// Store network manager.
		ECS::setCtx<Network::NetworkManager>();
		// Store settings manager.
		ECS::setCtx<Settings::SettingsManager>();
		// Store thread manager.
		ECS::setCtx<Threading::ThreadManager>();
		// Store uid manager.
		ECS::setCtx<Uid::UIDManager>();
		// Store pass manager.
		ECS::setCtx<Pass::PassManager>();

		VTX_DEBUG( "Init application" );

		// Load preset libraries.
		// TODO: move.
		auto & libRepresentations
			= LIBRARY().load<Library::Preset::Representation>( Filesystem::getRepresentationsDir() );
		auto & preset = libRepresentations.createPreset( "Sticks" );
		preset.setData( App::Library::Preset::Representations::STICKS );
		preset = libRepresentations.createPreset( "Balls and sticks" );
		preset.setData( App::Library::Preset::Representations::BALLS_AND_STICKS );
		preset = libRepresentations.createPreset( "Van der Waals" );
		preset.setData( App::Library::Preset::Representations::VAN_DER_WAALS );
		preset = libRepresentations.createPreset( "Ribbons" );
		preset.setData( App::Library::Preset::Representations::RIBBONS );
		preset = libRepresentations.createPreset( "SES" );
		preset.setData( App::Library::Preset::Representations::SES );

		auto & libColorLayouts	 = LIBRARY().load<Library::Preset::ColorLayout>( Filesystem::getColorLayoutsDir() );
		auto & libRenderSettings = LIBRARY().load<Library::Preset::RenderSettings>( Filesystem::getEffectsDir() );

		// Load settings.
		Settings::initSettings();

		// Register loop events.
		// onPostUpdate += []( const float p_elapsedTime ) { THREAD().lateUpdate(); };

		// Initialize python interpretor.
		/*
		INTERPRETOR().subscribe(
			[]( VTX::PythonBinding::Interpretor & p_interpretor )
			{
				p_interpretor.add( VTX::App::PythonBinding::VTXAppBinder() );
				p_interpretor.add( VTX::App::PythonBinding::RunScript() );
			}
		);
		*/

		// Creates entites/components.
		// Scene.
		_scene		= REG().create();
		auto & aabb = REG().emplace<Util::Math::AABB>( _scene ); // TODO: update from system aabbs.
		REG().emplace<Library::Preset::ColorLayout>( _scene, libColorLayouts.getPreset( "Default" ) );
		REG().emplace<Library::Preset::RenderSettings>( _scene, libRenderSettings.getPreset( "Default" ) );
		REG().emplace<Library::Preset::Representation>( _scene, libRepresentations.getPreset( "Default" ) );

		// Camera.
		_camera = REG().create();
		REG().emplace<Util::Math::Transform>( _camera );
		REG().emplace<Scene::Camera>( _camera );
	}

	VTXApp::~VTXApp()
	{
		VTX_INFO( "Stopping application" );

		// SCENE().reset();
		RENDERER().clean();

		//// Prevent events throw for nothing when quitting app
		// Old::Manager::EventManager::get().freezeEvent( true );
		//  Manager::WorkerManager::get().stopAll();

		//_setting.backup();

		// VTX::MVC_MANAGER().deleteModel( _representationLibrary );
		// VTX::MVC_MANAGER().deleteModel( _renderEffectLibrary );

		// Old::Selection::SelectionManager::get().deleteModel();
	}

	void VTXApp::start()
	{
		VTX_INFO( "Starting application: {}", ECS::getCtx<Args>().toString() );

		// Build the renderer (graphic api backend context ready).
		auto & renderer = RENDERER();

		if ( ECS::getCtx<Args>().has( ARG_NO_GRAPHICS ) )
		{
			VTX_WARNING( "No graphics" );
			renderer.setDefault();
		}
		else
		{
			try
			{
				renderer.setOpenGL45( Filesystem::getShadersDir() );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Failed to build renderer: {}", p_e.what() );
				renderer.setDefault();
				HUB().trigger<Events::ApplicationError>(
					"Unable to create OpenGL 4.5 context. Update your drivers and check your hardware compatibility."
				);
			}
		}

		// Add passes.
		// Camera updater.
		PASS().addPass<Pass::CameraUpdater>( _camera );
		// Trackball controller.
		// TODO: store current controller in settings?
		// PASS().addPass<Pass::Controller::Trackball>( _camera );
		ACTION().execute<Action::Controller::SetCameraController<Pass::Controller::Trackball>>();

		// ?
		// Internal::initSettings( App::SETTINGS() );

		// ACTION().execute<Action::Mode::SetMode<Mode::Visualization>>();
		HUB().trigger<Events::ApplicationStart>();

		// Updater.
		// UPDATER().onUpdateAvailable += []( const uint, const uint, const uint ) { UPDATER().downloadUpdate(); };

		if ( not ECS::getCtx<Args>().has( ARG_NO_UPDATE ) )
		{
			// UPDATER().checkForUpdate();
		}

		//_handleArgs( _args );
	}

	void VTXApp::_handleArgs( const Args & p_args )
	{
		// TODO: load pdb automatically or python script.

		/*
		using FILE_TYPE_ENUM = IO::Internal::Filesystem::FILE_TYPE_ENUM;
		for ( const auto arg : args.all() )
		{
			// If argument is an existing file
			if ( std::filesystem::exists( arg ) )
			{
				const FilePath		 path	  = FilePath( arg );
				const FILE_TYPE_ENUM fileType = IO::Internal::Filesystem::getFileTypeFromFilePath( path );

				try
				{
					switch ( fileType )
					{
					case FILE_TYPE_ENUM::MOLECULE:
					case FILE_TYPE_ENUM::TRAJECTORY:
						App::ACTION().execute<App::Action::Scene::LoadSystem>( arg );
						break;

					case FILE_TYPE_ENUM::SCENE:
						App::ACTION().execute<App::Action::Application::OpenScene>( arg );
						break;

					case FILE_TYPE_ENUM::SCRIPT:
						// App::VTX_ACTION().execute<PythonBinding::Action::RunScript>( arg );
						break;
					}
				}
				catch ( const IOException & p_e )
				{
					VTX_ERROR( "Can't open file '{}' : {}.", arg, p_e.what() );
				}
			}
			// If argument is a system name.
			else if ( arg.size() == 4 )
			{
				// Check only letter and number.
				if ( std::all_of( arg.begin(), arg.end(), []( const char c ) { return std::isalnum( c ); } ) )
				{
					App::ACTION().execute<App::Action::Scene::DownloadSystem>(
						arg, std::string( arg ) + ".pdb"
					);
				}
				else
				{
					VTX_WARNING( "Argument '{}' is not a valid system name.", arg );
				}
			}
			else
			{
				VTX_WARNING( "Argument '{}' is not valid.", arg );
			}
		}
		*/
	}

	//	bool VTXApp::hasAnyModifications() const
	//	{
	//		const bool hasSavePath	   = !getScenePathData().getCurrentPath().empty();
	//		const bool sceneIsEmpty	   = getScene().isEmpty();
	//		const bool sceneHasChanged = getScenePathData().sceneHasModifications();
	// #ifdef VTX_PRODUCTION
	//		return ( !hasSavePath && !sceneIsEmpty ) || ( hasSavePath && sceneHasChanged );
	// #else
	//		return hasSavePath && sceneHasChanged;
	// #endif
	//	}

} // namespace VTX::App
