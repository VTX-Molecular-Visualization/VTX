#ifndef __VTX_APP__
#define __VTX_APP__

#ifdef _MSC_VER
#pragma once

#endif
#include "action/action_manager.hpp"
#include "action/base_action.hpp"
#include "event/event.hpp"
#include "object3d/scene.hpp"
#include "renderer/base_renderer.hpp"
#include "setting.hpp"
#include "state/machine.hpp"
#include "tool/chrono.hpp"
#include "ui/user_interface.hpp"
#include "util/logger.hpp"
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
		void fireUIEvent( const Event::EVENT_UI, void * const = nullptr ) const;
		void action( Action::BaseAction * const ) const;
		void action( const std::string & ) const;
		bool canUndo() const;
		void undo() const;
		bool canRedo() const;
		void redo() const;

		void addThread( std::thread * const );

		void initRenderer() const;

		inline Object3D::Scene &		getScene() { return *_scene; }
		inline Renderer::BaseRenderer & getRenderer() { return *_renderer; }
		inline GL::GLSLProgramManager & getProgramManager() { return _renderer->getProgramManager(); }
		inline UI::UserInterface &		getUI() { return *_ui; }

	  private:
		static bool				 _isRunning;
		Tool::Chrono			 _chrono		= Tool::Chrono();
		double					 _timeTotal		= 0.f;
		double					 _timeDelta		= 0.f;
		UI::UserInterface *		 _ui			= nullptr;
		State::Machine *		 _stateMachine	= nullptr;
		Object3D::Scene *		 _scene			= nullptr;
		Renderer::BaseRenderer * _renderer		= nullptr;
		Action::ActionManager *	 _actionManager = nullptr;

		std::vector<std::thread *> _threads = std::vector<std::thread *>();

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
		void _handleEvent( const SDL_Event & );
	};
} // namespace VTX

#endif
