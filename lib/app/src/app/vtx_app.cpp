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
#include "app/helper/preset.hpp"
#include "app/input/input_manager.hpp"
#include "app/network/network_manager.hpp"
#include "app/pass/camera_updater.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/pass/scene_updater.hpp"
#include "app/pass/system_updater.hpp"
#include "app/pass/trajectory_updater.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/python_binding/python_binding.hpp"
#include "app/python_binding/run_script.hpp"
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
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
	constexpr size_t WIDTH_DEFAULT	= 1280;
	constexpr size_t HEIGHT_DEFAULT = 720;

} // namespace

namespace VTX::App
{

	VTXApp::VTXApp( const Args & p_args )
	{
		// Set global registry.
		ECS::setRegistry( _registry );

		// Store args.
		ECS::setCtx<Args>( p_args );
		// Session.
		ECS::setCtx<Session>();
		// Logger.
		LOGGER::init( SESSION().getLogsDir(), ARGS().has( ARG_DEBUG ) );
	}

	VTXApp::~VTXApp()
	{
		try
		{
			ECS::removeCtx<PythonBinding::Interpretor>();
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Exception during interpretor cleanup: {}", p_e.what() );
		}
		catch ( ... )
		{
			VTX_ERROR( "Unknown exception during interpretor cleanup" );
		}
		LOGGER().stop();
	}

	void VTXApp::start()
	{
		startServices();
		createInitialEntities();

		auto & renderer = RENDERER();
		if ( ECS::getCtx<Args>().has( ARG_NO_GRAPHICS ) )
		{
			VTX_WARNING( "No graphics backend initialization" );
			renderer.setDefault();
		}
		else
		{
			VTX_WARNING( "No UI graphics context available, using default backend" );
			renderer.setDefault();
		}

		finishStartup();

		_handleArgs();
	}

	void VTXApp::startServices()
	{
		VTX_INFO( ARGS().toString() );
		SESSION().print();

		ECS::setCtx<Util::EventHub>();
		ECS::setCtx<Util::Monitoring::Stats>();
		ECS::setCtx<Renderer::Renderer>();
		ECS::setCtx<Action::ActionManager>();
		ECS::setCtx<Input::InputManager>();
		ECS::setCtx<Network::NetworkManager>();
		ECS::setCtx<Settings::SettingsManager>();

		ECS::setCtx<Threading::ThreadManager>();
		ECS::setCtx<Uid::UIDManager>();
		ECS::setCtx<Pass::PassManager>();

		Settings::initSettings();

		try
		{
			ECS::setCtx<PythonBinding::Interpretor>();
			INTERPRETOR().subscribe(
				[]( VTX::PythonBinding::Interpretor & p_interpretor )
				{
					p_interpretor.add( VTX::App::PythonBinding::VTXAppBinder() );
					p_interpretor.add( VTX::App::PythonBinding::RunScript() );
				}
			);
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Failed to initialize python interpretor: {}", p_e.what() );
		}
	}

	void VTXApp::createInitialEntities()
	{
		const ECS::Entity sceneEnt = _registry.create();
		_registry.emplace<Scene::TagRoot>( sceneEnt );
		_registry.emplace<Util::Math::AABB>( sceneEnt );

		const ECS::Entity cameraEnt = _registry.create();
		_registry.emplace<Util::Math::Transform>( cameraEnt );
		_registry.emplace<Renderer::Camera>( cameraEnt );
		ACTION().execute<App::Action::Application::Resize>( WIDTH_DEFAULT, HEIGHT_DEFAULT );

		ACTION().execute<Action::Preset::CreateDefault<Renderer::Color::Layout>>();
		ACTION().execute<Action::Preset::CreateDefault<Renderer::Representation>>();
		ACTION().execute<Action::Preset::CreateDefault<Renderer::GraphicsConfig>>();

		PASS().addPass<Pass::SceneUpdater>( sceneEnt );

		PASS().addPass<Pass::CameraUpdater>( cameraEnt );
		PASS().addPass<Pass::SystemUpdater>();
		PASS().addPass<Pass::TrajectoryUpdater>();

		if ( SETTINGS().getValue<Renderer::PROJECTION>( Settings::Camera::PROJECTION_KEY )
			 == Renderer::PROJECTION::PERSPECTIVE )
		{
			ACTION().execute<Action::Camera::SetProjectionMode<Renderer::PROJECTION::PERSPECTIVE>>();
		}
		else
		{
			ACTION().execute<Action::Camera::SetProjectionMode<Renderer::PROJECTION::ORTHOGRAPHIC>>();
		}

		ACTION().execute<Action::Controller::SetCameraController<Action::Controller::E_CONTROLLER::TRACKBALL>>();
	}

	void VTXApp::finishStartup()
	{
		const std::string runtimePythonVersion = INTERPRETOR().getRuntimePythonVersion();
		if ( not runtimePythonVersion.empty() )
		{
			HUB().trigger<Events::PythonInitialized>( runtimePythonVersion );
			VTX_INFO( "Python interpretor initialized" );
		}

		const ECS::Entity defaultGraphicsConfig
			= Helper::Preset::getByName<Renderer::GraphicsConfig>( "Default" )
				  .value_or( ECS::getFirstEntityOnlyWithComponents<Preset::Name, Renderer::GraphicsConfig>() );
		ACTION().execute<Action::Scene::SetGraphicsConfig>( defaultGraphicsConfig );

		const ECS::Entity defaultColorLayout = Helper::Preset::getByName<Renderer::Color::Layout>( "JMol" ).value_or(
			ECS::getFirstEntityOnlyWithComponents<Preset::Name, Renderer::Color::Layout>()
		);
		ACTION().execute<Action::Scene::SetColorLayout>( defaultColorLayout );

		HUB().trigger<Events::ApplicationStart>();

		if ( not ARGS().has( ARG_NO_UPDATE ) )
		{
			SESSION().checkForUpdate();
		}
	}

	void VTXApp::_handleArgs()
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
