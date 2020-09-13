#ifndef __VTX_APP__
#define __VTX_APP__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/action_manager.hpp"
#include "event/event_manager.hpp"
#include "renderer/base_renderer.hpp"
#include "setting.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "worker/worker_manager.hpp"

namespace VTX
{
	// Forward declaration to avoid circular dependencies and reduce compile time (maybe?)
	class Setting;
	namespace State
	{
		class StateMachine;
	}
	namespace Selection
	{
		class SelectionManager;
	}
	namespace Renderer
	{
		class GL;
		namespace GLSL
		{
			class ProgramManager;
		}
#ifdef CUDA_DEFINED
		class RayTracer;
#endif
#ifdef OPTIX_DEFINED
		namespace Optix
		{
			class OptixRayTracer;
		}
#endif
	} // namespace Renderer
	namespace Object3D
	{
		class Scene;
	}
	namespace UI
	{
		class UserInterface;
	}

	class VTXApp final
	{
	  public:
		inline static VTXApp & get()
		{
			static VTXApp instance;
			return instance;
		}
		static bool isRunning() { return VTXApp::_isRunning; }

		void start();
		void stop();
		void goToState( const std::string &, void * const = nullptr );
		void renderScene() const { _renderer->renderFrame( *_scene ); }

		inline Setting &					  getSetting() { return _setting; }
		inline const Setting &				  getSetting() const { return _setting; }
		inline Tool::Logger &				  getLogger() { return _logger; }
		inline const Tool::Logger &			  getLogger() const { return _logger; }
		inline Object3D::Scene &			  getScene() { return *_scene; }
		inline const Object3D::Scene &		  getScene() const { return *_scene; }
		inline Renderer::BaseRenderer &		  getRenderer() { return *_renderer; }
		inline const Renderer::BaseRenderer & getRenderer() const { return *_renderer; }
		inline Renderer::GL &				  getRendererGL() { return *_rendererGL; }
		inline const Renderer::GL &			  getRendererGL() const { return *_rendererGL; }
#ifdef CUDA_DEFINED
		inline Renderer::RayTracer &	   getRendererRT() { return *_rendererRT; }
		inline const Renderer::RayTracer & getRendererRT() const { return *_rendererRT; }
#endif
#ifdef OPTIX_DEFINED
		inline Renderer::Optix::OptixRayTracer &	   getRendererOptix() { return *_rendererOptix; }
		inline const Renderer::Optix::OptixRayTracer & getRendererOptix() const { return *_rendererOptix; }
#endif
		inline Renderer::GLSL::ProgramManager &		  getProgramManager() { return _renderer->getProgramManager(); }
		inline const Renderer::GLSL::ProgramManager & getProgramManager() const
		{
			return _renderer->getProgramManager();
		}
		inline UI::UserInterface &				   getUI() { return *_ui; }
		inline const UI::UserInterface &		   getUI() const { return *_ui; }
		inline State::StateMachine &			   getStateMachine() { return *_stateMachine; }
		inline const State::StateMachine &		   getStateMachine() const { return *_stateMachine; }
		inline Action::ActionManager &			   getActionManager() { return *_actionManager; }
		inline const Action::ActionManager &	   getActionManager() const { return *_actionManager; }
		inline Event::EventManager &			   getEventManager() { return *_eventManager; }
		inline const Event::EventManager &		   getEventManager() const { return *_eventManager; }
		inline Worker::WorkerManager &			   getWorkerManager() { return *_workerManager; }
		inline const Worker::WorkerManager &	   getWorkerManager() const { return *_workerManager; }
		inline Selection::SelectionManager &	   getSelectionManager() { return *_selectionManager; }
		inline const Selection::SelectionManager & getSelectionManager() const { return *_selectionManager; }

		void switchRenderer( const Renderer::MODE );

	  private:
		static bool				 _isRunning;
		Setting					 _setting	   = Setting();
		Tool::Logger			 _logger	   = Tool::Logger();
		Tool::Chrono			 _chrono	   = Tool::Chrono();
		UI::UserInterface *		 _ui		   = nullptr;
		State::StateMachine *	 _stateMachine = nullptr;
		Object3D::Scene *		 _scene		   = nullptr;
		Renderer::BaseRenderer * _renderer	   = nullptr;
		Renderer::GL *			 _rendererGL   = nullptr;
#ifdef CUDA_DEFINED
		Renderer::RayTracer * _rendererRT = nullptr;
#endif
#ifdef OPTIX_DEFINED
		Renderer::Optix::OptixRayTracer * _rendererOptix = nullptr;
#endif
		Action::ActionManager *		  _actionManager	= nullptr;
		Event::EventManager *		  _eventManager		= nullptr;
		Worker::WorkerManager *		  _workerManager	= nullptr;
		Selection::SelectionManager * _selectionManager = nullptr;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
	};

	// TODO: check const
	inline Setting & VTX_SETTING() { return VTXApp::get().getSetting(); }
	inline void		 VTX_EVENT( VTX::Event::VTXEvent * const p_event )
	{
		VTXApp::get().getEventManager().fireEvent( p_event );
	}
	inline void VTX_ACTION( VTX::Action::BaseAction * const p_action, const bool p_force = false )
	{
		VTXApp::get().getActionManager().execute( p_action, p_force );
	}
	inline void VTX_ACTION( const std::string & p_action, const bool p_force = false )
	{
		VTXApp::get().getActionManager().execute( p_action, p_force );
	}
	// TODO: will be deleted when all workers will be threaded.
	inline void VTX_WORKER( VTX::Worker::BaseWorker * const p_worker )
	{
		VTXApp::get().getWorkerManager().run( p_worker );
	}
	inline void VTX_WORKER( VTX::Worker::BaseWorker * const		  p_worker,
							const Worker::CallbackSuccess * const p_success,
							const Worker::CallbackError * const	  p_error )
	{
		VTXApp::get().getWorkerManager().run( p_worker, p_success, p_error );
	}
	inline void VTX_DEBUG( const std::string & p_str ) { VTXApp::get().getLogger().logDebug( p_str ); }
	inline void VTX_INFO( const std::string & p_str ) { VTXApp::get().getLogger().logInfo( p_str ); }
	inline void VTX_WARNING( const std::string & p_str ) { VTXApp::get().getLogger().logWarning( p_str ); }
	inline void VTX_ERROR( const std::string & p_str ) { VTXApp::get().getLogger().logError( p_str ); }
	inline void VTX_CONSOLE( const std::string & p_str ) { std::cout << p_str << std::endl; }

} // namespace VTX

#endif
