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
		void initRenderer() const;
		void renderScene() const { _renderer->render( *_scene ); }

		inline Object3D::Scene &				getScene() { return *_scene; }
		inline Renderer::BaseRenderer &			getRenderer() { return *_renderer; }
		inline Renderer::GLSL::ProgramManager & getProgramManager() { return _renderer->getProgramManager(); }
		inline UI::UserInterface &				getUI() { return *_ui; }
		inline Action::ActionManager &			getActionManager() { return *_actionManager; }
		inline Event::EventManager &			getEventManager() { return *_eventManager; }
		inline Worker::WorkerManager &			getWorkerManager() { return *_workerManager; }

	  private:
		static bool					  _isRunning;
		Tool::Chrono				  _chrono			= Tool::Chrono();
		double						  _timeTotal		= 0.f;
		double						  _timeDelta		= 0.f;
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
} // namespace VTX

#endif
