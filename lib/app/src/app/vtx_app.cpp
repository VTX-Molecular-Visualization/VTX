#include "app/vtx_app.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/application.hpp"
#include "app/action/camera.hpp"
#include "app/action/color_layout.hpp"
#include "app/action/controller.hpp"
#include "app/action/graphics_config.hpp"
#include "app/action/preset.hpp"
#include "app/action/representation.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/filesystem.hpp"
#include "app/input/input_manager.hpp"
#include "app/network/network_manager.hpp"
#include "app/pass/camera_updater.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/pass/scene_updater.hpp"
#include "app/preset/instance.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/python_binding/python_binding.hpp"
#include "app/python_binding/run_script.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include "app/threading/thread_manager.hpp"
#include "app/uid/uid_manager.hpp"
#include <exception>
#include <python_binding/interpretor.hpp>
#include <renderer/renderer.hpp>
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
		ECS::setCtx<Renderer::Renderer>().setDefault();
		// Store action manager.
		ECS::setCtx<Action::ActionManager>();
		// Store input manager.
		ECS::setCtx<Input::InputManager>();
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
		// Store python interpretor.
		ECS::setCtx<PythonBinding::Interpretor>();

		// Load settings.
		Settings::initSettings();
		auto & settings = SETTINGS();

		// Scene.
		_scene = _registry.create();
		_registry.emplace<Scene::TagRoot>( _scene );
		_registry.emplace<Util::Math::AABB>( _scene );

		// Camera.
		_camera = _registry.create();
		_registry.emplace<Util::Math::Transform>( _camera );
		_registry.emplace<Renderer::Camera>( _camera );

		// Initialize python interpretor.
		INTERPRETOR().subscribe(
			[]( VTX::PythonBinding::Interpretor & p_interpretor )
			{
				p_interpretor.add( VTX::App::PythonBinding::VTXAppBinder() );
				p_interpretor.add( VTX::App::PythonBinding::RunScript() );
			}
		);
	}

	VTXApp::~VTXApp() { ECS::removeCtx<PythonBinding::Interpretor>(); }

	void VTXApp::start()
	{
		VTX_INFO( "Starting application: {}", ECS::getCtx<Args>().toString() );

		// Build the renderer (graphic api backend context ready).
		auto & renderer = RENDERER();

		if ( ECS::getCtx<Args>().has( ARG_NO_GRAPHICS ) )
		{
			VTX_WARNING( "No graphics" );
			// Default state set in constructor.
			// renderer.setDefault();
			// Resize to minimal size to avoid issues.
			ACTION().execute<Action::Application::Resize>( 1, 1 );
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

		// Create default presets.
		ACTION().execute<Action::Preset::CreateDefault<Renderer::Color::Layout>>();
		ACTION().execute<Action::Preset::CreateDefault<Renderer::Representation>>();
		ACTION().execute<Action::Preset::CreateDefault<Renderer::GraphicsConfig>>();

		// Launch passes.
		PASS().addPass<Pass::SceneUpdater>( _scene );
		PASS().addPass<Pass::CameraUpdater>( _camera );

		// Set preset instances.
		ACTION().execute<Action::Preset::SetCurrent<Renderer::Color::Layout>>(
			ECS::getFirstEntityOnlyWithComponents<Preset::Name, Renderer::Color::Layout>()
		);
		ACTION().execute<Action::Preset::SetCurrent<Renderer::Representation>>(
			ECS::getFirstEntityOnlyWithComponents<Preset::Name, Renderer::Representation>()
		);
		ACTION().execute<Action::Preset::SetCurrent<Renderer::GraphicsConfig>>(
			ECS::getFirstEntityOnlyWithComponents<Preset::Name, Renderer::GraphicsConfig>()
		);

		// TODO: at setting loading.
		// Camera projection.
		if ( SETTINGS().getValue<Renderer::PROJECTION>( Settings::Camera::PROJECTION_KEY )
			 == Renderer::PROJECTION::PERSPECTIVE )
		{
			ACTION().execute<Action::Camera::SetProjectionMode<Renderer::PROJECTION::PERSPECTIVE>>();
		}
		else
		{
			ACTION().execute<Action::Camera::SetProjectionMode<Renderer::PROJECTION::ORTHOGRAPHIC>>();
		}

		// Trackball controller.
		ACTION().execute<Action::Controller::SetCameraController<Pass::Controller::Trackball>>();

		// Trigger application start event.
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
