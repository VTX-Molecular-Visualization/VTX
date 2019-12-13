#ifndef __VTX_APP__
#define __VTX_APP__

#ifdef _MSC_VER
#pragma once

#endif
#include "event/event.hpp"
#include "object3d/scene.hpp"
#include "renderer/base_renderer.hpp"
#include "settings.hpp"
#include "state/state_machine.hpp"
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
		void goToState( const State::STATE_NAME );
		void fireUIEvent( const Event::EVENT_UI, void * const ) const;
		// void addThread( std::thread * const );

		const std::shared_ptr<UI::BaseComponent> getUIComponentByName( const UI::COMPONENT_NAME ) const;
		void									 setTheme() const;
		void									 initRenderer() const;
		void									 setRendererShading() const;

		inline Object3D::Scene &			getScene() { return *_scene; }
		inline Shader::GLSLProgramManager & getProgramManager() { return _renderer->getProgramManager(); }

		inline double getTimeTotal() { return _timeTotal; }
		inline double getTimeLast() { return _timeLast; }
		inline double getTimeLastUI() { return _timeLastUI; }
		inline double getTimeLastRenderer() { return _timeLastRenderer; }

	  private:
		static bool				   _isRunning;
		Tool::Chrono			   _chrono			 = Tool::Chrono();
		double					   _timeTotal		 = 0.f;
		double					   _timeLast		 = 0.f;
		double					   _timeLastUI		 = 0.f;
		double					   _timeLastRenderer = 0.f;
		UI::UserInterface *		   _ui				 = nullptr;
		State::StateMachine *	   _stateMachine	 = nullptr;
		Object3D::Scene *		   _scene			 = nullptr;
		Renderer::BaseRenderer *   _renderer		 = nullptr;
		std::vector<std::thread *> _threads			 = std::vector<std::thread *>();

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
		void _handleEvent( const SDL_Event & );
	};
} // namespace VTX

#endif
