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
#include "renderer/base_renderer.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "tool/chrono.hpp"
#include "ui/user_interface.hpp"
#include "util/logger.hpp"
#include "worker/worker_manager.hpp"
#include <thread>
#include <vector>

namespace VTX
{
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
		void renderScene() const { _renderer->render( *_scene ); }

		inline Object3D::Scene &					  getScene() { return *_scene; }
		inline const Object3D::Scene &				  getScene() const { return *_scene; }
		inline Renderer::BaseRenderer &				  getRenderer() { return *_renderer; }
		inline const Renderer::BaseRenderer &		  getRenderer() const { return *_renderer; }
		inline Renderer::GLSL::ProgramManager &		  getProgramManager() { return _renderer->getProgramManager(); }
		inline const Renderer::GLSL::ProgramManager & getProgramManager() const
		{
			return _renderer->getProgramManager();
		}
		inline UI::UserInterface &				   getUI() { return *_ui; }
		inline const UI::UserInterface &		   getUI() const { return *_ui; }
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
		Tool::Chrono				  _chrono			= Tool::Chrono();
		UI::UserInterface *			  _ui				= nullptr;
		State::StateMachine *		  _stateMachine		= nullptr;
		Object3D::Scene *			  _scene			= nullptr;
		Renderer::BaseRenderer *	  _renderer			= nullptr;
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

	inline void VTX_EVENT( VTX::Event::VTXEvent * const p_event )
	{
		VTXApp::get().getEventManager().fireEvent( p_event );
	}
	inline void VTX_ACTION( VTX::Action::BaseAction * const p_action )
	{
		VTXApp::get().getActionManager().execute( p_action );
	}
	inline void VTX_ACTION( std::string & p_action ) { VTXApp::get().getActionManager().execute( p_action ); }
} // namespace VTX

#endif
