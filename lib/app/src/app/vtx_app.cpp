#include "app/vtx_app.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/application.hpp"
#include "app/action/camera.hpp"
#include "app/action/color_layout.hpp"
#include "app/action/controller.hpp"
#include "app/action/graphics_config.hpp"
#include "app/action/io.hpp"
#include "app/action/preset.hpp"
#include "app/action/representation.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/helper/preset.hpp"
#include "app/input/input_manager.hpp"
#include "app/network/network_manager.hpp"
#include "app/pass/camera_updater.hpp"
#include "app/pass/mesh_updater.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/pass/scene_updater.hpp"
#include "app/pass/system_updater.hpp"
#include "app/pass/trajectory_updater.hpp"
#if VTX_PYTHON_BINDING
#include "app/python_binding/app_binder.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/python_binding/run_script.hpp"
#endif
#include "app/scene/tag_root.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include "app/setting/accessibility.hpp"
#include "app/setting/controller.hpp"
#include "app/uid/uid_manager.hpp"
#include <exception>
#include <util/thread/thread_manager.hpp>
#if VTX_PYTHON_BINDING
#include <python_binding/interpretor.hpp>
#endif
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

	VTXApp::VTXApp( Arguments && p_args )
	{
		bool debug = p_args.debug;

		// Set global registry.
		ECS::setRegistry( _registry );

		// Store args.
		ECS::setCtx<Arguments>( std::move( p_args ) );
		// Session.
		ECS::setCtx<Session>();
		// Logger.
		LOGGER::init( SESSION().getLogsDir(), debug );
	}

	VTXApp::~VTXApp()
	{
		if ( ECS::hasCtx<Action::ActionManager>() )
		{
			ACTION().shutdown();
		}
#if VTX_PYTHON_BINDING
		try
		{
			if ( ECS::hasCtx<PythonBinding::Interpretor>() )
			{
				ECS::removeCtx<PythonBinding::Interpretor>();
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Exception during interpretor cleanup: {}", p_e.what() );
		}
		catch ( ... )
		{
			VTX_ERROR( "Unknown exception during interpretor cleanup" );
		}
#endif
		if ( ECS::hasCtx<Pass::PassManager>() )
		{
			ECS::removeCtx<Pass::PassManager>();
		}
		try
		{
			if ( ECS::hasCtx<Util::Thread::ThreadManager>() )
			{
				ECS::removeCtx<Util::Thread::ThreadManager>();
			}
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "Exception during thread manager cleanup: {}", p_e.what() );
		}
		catch ( ... )
		{
			VTX_ERROR( "Unknown exception during thread manager cleanup" );
		}

		LOGGER().flush();
	}

	void VTXApp::start()
	{
		startServices();
		createInitialEntities();

		auto & renderer = RENDERER();
		if ( ECS::getCtx<Arguments>().noGraphics )
		{
			VTX_WARNING( "No graphics backend initialization" );
			renderer.setDefault(); // TODO : SetDefault in both cases ?
		}
		else
		{
			VTX_WARNING( "No UI graphics context available, using default backend" );
			renderer.setDefault();
		}

		finishStartup();

		handleArgs();
	}

	void VTXApp::startServices()
	{
		VTX_INFO( toString( ARGS() ) );
		SESSION().print();

		ECS::setCtx<Util::EventHub>();
		ECS::setCtx<Util::Monitoring::Stats>();
		ECS::setCtx<Renderer::Renderer>();
		ECS::setCtx<Action::ActionManager>();
		ECS::setCtx<Input::InputManager>();
		ECS::setCtx<Network::NetworkManager>();
		ECS::setCtx<Util::Thread::ThreadManager>();
		THREAD().setDefaultProgressCallback(
			[]( const Util::Thread::BaseThread & p_thread )
			{
				HUB().trigger<Events::ThreadProgress>(
					p_thread.getId(), p_thread.getProgress(), p_thread.getProgressText()
				);
			}
		);
		THREAD().setDefaultTerminatedCallback(
			[]( const Util::Thread::BaseThread & p_thread )
			{ HUB().trigger<Events::ThreadTerminated>( p_thread.getId(), p_thread.isManuallyStopped() ); }
		);
		ECS::setCtx<Uid::UIDManager>();
		ECS::setCtx<Pass::PassManager>();
		ECS::setCtx<Core::ChemDB::Category::Dictionary>( Core::ChemDB::Category::createDefaultDictionary() );

		const Entity accessibilityEnt = _registry.create();
		_registry.emplace<Setting::Accessibility>( accessibilityEnt );

#if VTX_PYTHON_BINDING
		if ( not ARGS().noPython )
		{
			try
			{
				ECS::setCtx<PythonBinding::Interpretor>();
				INTERPRETOR().subscribe(
					[]( VTX::PythonBinding::Interpretor & p_interpretor )
					{
						p_interpretor.add( VTX::App::PythonBinding::AppBinder() );
						p_interpretor.add( VTX::App::PythonBinding::RunScript() );
					}
				);
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Failed to initialize python interpretor: {}", p_e.what() );
			}
		}
		else
#endif
		{
			VTX_INFO( "Python interpretor disabled" );
		}
	}

	void VTXApp::createInitialEntities()
	{
		// Scene.
		const Entity sceneEnt = _registry.create();
		_registry.emplace<Scene::TagRoot>( sceneEnt );
		_registry.emplace<Util::Math::AABB>( sceneEnt );

		// Camera.
		const Entity cameraEnt = _registry.create();
		_registry.emplace<Util::Math::Transform>( cameraEnt );
		_registry.emplace<Renderer::Camera>( cameraEnt );
		_registry.emplace<Setting::Controller>( cameraEnt );

		// Resize renderer.
		ACTION().execute<App::Action::Application::Resize>( WIDTH_DEFAULT, HEIGHT_DEFAULT );

		// Run passes.
		PASS().addPass<Pass::SceneUpdater>( sceneEnt );
		PASS().addPass<Pass::CameraUpdater>( cameraEnt );
		PASS().addPass<Pass::SystemUpdater>();
		PASS().addPass<Pass::MeshUpdater>();
		PASS().addPass<Pass::TrajectoryUpdater>();

		// Default presets.
		ACTION().execute<Action::Preset::CreateDefault<Renderer::Color::Layout>>();
		ACTION().execute<Action::Preset::CreateDefault<Renderer::Representation>>();
		ACTION().execute<Action::Preset::CreateDefault<Renderer::Material>>();
		ACTION().execute<Action::Preset::CreateDefault<Renderer::GraphicsConfig>>();
	}

	void VTXApp::finishStartup()
	{
#if VTX_PYTHON_BINDING
		if ( not ARGS().noPython )
		{
			const std::string runtimePythonVersion = INTERPRETOR().getRuntimePythonVersion();
			if ( not runtimePythonVersion.empty() )
			{
				HUB().trigger<Events::PythonInitialized>( runtimePythonVersion );
				VTX_INFO( "Python interpretor initialized" );
			}
		}
#endif

		const Entity defaultGraphicsConfig
			= Helper::Preset::getByName<Renderer::GraphicsConfig>( "Default" )
				  .value_or( ECS::getFirstEntityOnlyWithComponents<Generic::Name, Renderer::GraphicsConfig>() );
		ACTION().execute<Action::Scene::SetGraphicsConfig>( defaultGraphicsConfig );

		const Entity defaultColorLayout = Helper::Preset::getByName<Renderer::Color::Layout>( "JMol" ).value_or(
			ECS::getFirstEntityOnlyWithComponents<Generic::Name, Renderer::Color::Layout>()
		);
		ACTION().execute<Action::Scene::SetColorLayout>( defaultColorLayout );

		HUB().trigger<Events::ApplicationStart>();

		if ( not ARGS().noUpdates )
		{
			SESSION().checkForUpdate();
		}
	}

	void VTXApp::handleArgs()
	{
		const Arguments & args = ARGS();
		for ( auto & it_file : args.positionalFiles )
		{
			if ( it_file.empty() )
			{
				continue; // The user probably didn't do it on purpose so no need to complain here
			}

			if ( not std::filesystem::exists( it_file ) )
			{
				// Maybe there is a problem with the path so we need to tell 'em
				VTX_WARNING( "File <{}> not found. VTX will skip it.", it_file );
				continue;
			}

			Action::IO::Open openAction;
			ACTION().execute( openAction, it_file );
		}
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
