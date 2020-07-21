#ifndef __VTX_APP__
#define __VTX_APP__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/action_manager.hpp"
#include "action/base_action.hpp"
#include "event/event_manager.hpp"
#include "generic/factory.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/gl.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "tool/chrono.hpp"
#include "ui/user_interface.hpp"
#include "worker/loader.hpp"
#include "worker/worker_manager.hpp"
#include <nlohmann/json.hpp>
#include <thread>
#include <vector>

namespace VTX
{
	class Setting;

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
		void renderScene() const { _rendererGL->renderFrame( *_scene ); }

		inline Setting &							  getSetting() { return _setting; }
		inline const Setting &						  getSetting() const { return _setting; }
		inline Object3D::Scene &					  getScene() { return *_scene; }
		inline const Object3D::Scene &				  getScene() const { return *_scene; }
		inline Renderer::GL &						  getRendererGL() { return *_rendererGL; }
		inline const Renderer::GL &					  getRendererGL() const { return *_rendererGL; }
		inline Renderer::GLSL::ProgramManager &		  getProgramManager() { return _rendererGL->getProgramManager(); }
		inline const Renderer::GLSL::ProgramManager & getProgramManager() const
		{
			return _rendererGL->getProgramManager();
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

	  private:
		static bool					  _isRunning;
		Setting						  _setting			= Setting();
		Tool::Chrono				  _chrono			= Tool::Chrono();
		UI::UserInterface *			  _ui				= nullptr;
		State::StateMachine *		  _stateMachine		= nullptr;
		Object3D::Scene *			  _scene			= nullptr;
		Renderer::GL *				  _rendererGL		= nullptr;
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

	void to_json( nlohmann::json &, const VTXApp & );
	void from_json( const nlohmann::json &, VTXApp & );

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

} // namespace VTX

#endif
