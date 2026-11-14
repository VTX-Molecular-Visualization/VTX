#include "app/vtx_app.hpp"
#include "app/action/application.hpp"
#include "app/action/io.hpp"
#include "app/action/preset.hpp"
#include "app/action/scene.hpp"
#include "app/events.hpp"
#include "app/helper/preset.hpp"
#include "app/input/input_manager.hpp"
#include "app/pass/camera_updater.hpp"
#include "app/pass/mesh_updater.hpp"
#include "app/pass/pass_manager.hpp"
#include "app/pass/scene_updater.hpp"
#include "app/pass/system_updater.hpp"
#include "app/pass/trajectory_updater.hpp"
#include "app/pending_updates.hpp"
#include <util/action_manager.hpp>
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
#include <chrono>
#include <cstdio>
#include <exception>
#include <future>
#include <io/file_format.hpp>
#include <memory>
#include <tuple>
#include <util/chrono.hpp>
#include <util/event_hub.hpp>
#include <util/thread/thread_manager.hpp>
#if VTX_PYTHON_BINDING
#include <python_binding/interpretor.hpp>
#endif
#include <renderer/renderer.hpp>
#include <util/logger.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>

namespace
{
	constexpr size_t WIDTH_DEFAULT	= 1280;
	constexpr size_t HEIGHT_DEFAULT = 720;

} // namespace

namespace VTX::App
{
#if VTX_PYTHON_BINDING
	namespace
	{
		void runScriptUntilDone( VTXApp & p_app, const FilePath & p_path )
		{
			const auto promise = std::make_shared<std::promise<PythonBinding::Interpretor::AsyncJobResult>>();
			const std::future<PythonBinding::Interpretor::AsyncJobResult> future = promise->get_future();
			INTERPRETOR().runScript( p_path, promise );

			std::ignore = p_app.runUntilReady( future );
			p_app.update();
		}
	} // namespace
#endif

	VTXApp::VTXApp( Arguments && p_args )
	{
		const bool debug = p_args.debug;

		// Set global registry.
		ECS::setRegistry( _registry );

		// Store args.
		ECS::setCtx<Arguments>( std::move( p_args ) );
		// Event hub.
		ECS::setCtx<Util::EventHub>();
		// Session.
		ECS::setCtx<Session>();
		// Logger.
		LOGGER::init( SESSION().getLogsDir(), debug );
		LOGGER::setCallback( [ &hub = HUB() ]( const Util::LogInfo & p_logInfo )
							 { hub.enqueue<Util::LogInfo>( p_logInfo ); } );
	}

	VTXApp::~VTXApp()
	{
		if ( ECS::hasCtx<PendingUpdates>() )
		{
			unbindPending( PENDING_UPDATES() );
		}
		if ( ECS::hasCtx<Util::Thread::ThreadManager>() )
		{
			unbindPending( THREAD() );
		}
		if ( ECS::hasCtx<Renderer::Renderer>() )
		{
			unbindPending( RENDERER() );
		}
		unbindPending( HUB() );

#if VTX_PYTHON_BINDING
		if ( ECS::hasCtx<PythonBinding::Interpretor>() )
		{
			unbindPending( INTERPRETOR() );
			ECS::removeCtx<PythonBinding::Interpretor>();
		}
#endif
		// Destroy component users before entt registry destruction.
		if ( ECS::hasCtx<Pass::PassManager>() )
		{
			ECS::removeCtx<Pass::PassManager>();
		}
		if ( ECS::hasCtx<Util::ActionManager>() )
		{
			ECS::removeCtx<Util::ActionManager>();
		}
		if ( ECS::hasCtx<Util::Thread::ThreadManager>() )
		{
			ECS::removeCtx<Util::Thread::ThreadManager>();
		}

		if ( ECS::hasCtx<Session>() )
		{
			ECS::removeCtx<Session>();
		}

		try
		{
			LOGGER::stop();
		}
		catch ( ... )
		{
			std::ignore = std::fputs( "Failed to stop logger\n", stderr );
		}
	}

	void VTXApp::start()
	{
		startServices();
		setFrameScheduler( &VTXApp::_wake, this );
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

		ECS::setCtx<PendingUpdates>();
		bindPending<E_PENDING_UPDATE::EVENTS>( HUB() );
		ECS::setCtx<Renderer::Renderer>();
		bindPending<E_PENDING_UPDATE::RENDER>( RENDERER() );
		RENDERER().onReady
			= []( const Renderer::StructInfos & p_infos ) { HUB().trigger<Events::RendererReady>( p_infos ); };
		ECS::setCtx<Util::Thread::ThreadManager>();
		bindPending<E_PENDING_UPDATE::THREADS>( THREAD() );
		if ( ARGS().noGui )
		{
			ECS::setCtx<Util::ActionManager>();
		}
		else
		{
			ECS::setCtx<Util::ActionManager>( THREAD() );
		}
		ECS::setCtx<Input::InputManager>();
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
			{
				HUB().trigger<Events::ThreadTerminated>(
					p_thread.getId(), p_thread.isManuallyStopped(), p_thread.hasFailed()
				);
			}
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
				bindPending<E_PENDING_UPDATE::THREADS>( INTERPRETOR() );
				INTERPRETOR().post(
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
			catch ( ... )
			{
				VTX_ERROR( "Failed to initialize python interpretor: unknown exception" );
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
		if ( not ARGS().noPython && ECS::hasCtx<PythonBinding::Interpretor>() )
		{
			INTERPRETOR().requestRuntimePythonVersion(
				[ &threadManager = THREAD() ]( std::string p_version )
				{
					threadManager.dispatch(
						[ version = std::move( p_version ) ]()
						{
							HUB().trigger<Events::PythonInitialized>( version );
							VTX_INFO( "Python interpretor initialized" );
						}
					);
				}
			);
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

		_chrono.start();
		_idle = true;
	}

	void VTXApp::handleArgs()
	{
		const Arguments & args = ARGS();
		for ( const auto & it_file : args.positionalFiles )
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

#if VTX_PYTHON_BINDING
			if ( args.noGui && ECS::hasCtx<PythonBinding::Interpretor>() && VTX::IO::isScriptFileFormat( it_file ) )
			{
				runScriptUntilDone( *this, it_file );
				continue;
			}
#endif
			Action::IO::Open openAction;
			ACTION().execute( openAction, it_file );
		}
	}

	void VTXApp::setFrameScheduler( const Util::AtomicCallback::Function p_function, void * const p_context )
	{
		PendingUpdates & pendingUpdates = PENDING_UPDATES();
		pendingUpdates.setOnPending( p_function, p_context );
		if ( p_function != nullptr )
		{
			pendingUpdates.mark( E_PENDING_UPDATE::ALL );
			p_function( p_context );
		}
	}

	void VTXApp::update() noexcept
	try
	{
		PendingUpdates & pendingUpdates = PENDING_UPDATES();
		if ( not pendingUpdates.any() )
		{
			_idle = true;
			return;
		}

		if ( _idle )
		{
			std::ignore = _chrono.intervalTime();
		}

		const E_PENDING_UPDATE pending = pendingUpdates.consume();
		_pipeline.process( pending, _chrono.intervalTime(), _chrono.elapsedTime() );
		_idle = not pendingUpdates.any();
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( "Application update failed: {}", p_e.what() );
	}
	catch ( ... )
	{
		VTX_ERROR( "Unknown application update failure" );
	}

	bool VTXApp::runUntilIdle( const std::optional<std::chrono::milliseconds> p_timeout )
	{
		return runUntil( []() { return not PENDING_UPDATES().any(); }, p_timeout );
	}

	bool VTXApp::_waitForPendingUpdates( const std::optional<std::chrono::steady_clock::time_point> & p_deadline )
	{
		std::unique_lock lock( _wakeMutex );
		const auto		 hasPendingUpdates = []() { return PENDING_UPDATES().any(); };
		if ( not p_deadline.has_value() )
		{
			_wakeCondition.wait( lock, hasPendingUpdates );
			return true;
		}
		return _wakeCondition.wait_until( lock, *p_deadline, hasPendingUpdates );
	}

	void VTXApp::_wake( void * const p_app ) noexcept
	{
		VTXApp * const app = static_cast<VTXApp *>( p_app );
		try
		{
			const std::scoped_lock lock( app->_wakeMutex );
		}
		catch ( ... )
		{
			LOGGER::logException( "Failed to wake main loop" );
		}
		app->_wakeCondition.notify_all();
	}
} // namespace VTX::App
