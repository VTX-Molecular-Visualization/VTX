/*
 * File: app.hpp
 * Author: Maxime MARIA
 */

#ifndef __APP_HPP__
#define __APP_HPP__

#include <iostream>
#include <string>

#include "GLSLProgramManager.hpp"
#include "io/filePath.hpp"
#include "io/ioUtils.hpp"
#include "renderer.hpp"
#include "scene.hpp"
#include "singletonClass.hpp"
#include "userInterface.hpp"
#include "window.hpp"

namespace vidocklab
{
	class App : public Singleton<App>
	{
	public:
		App() = default;
		~App() = default;

		void init(int argc, char *argv[]);
		void launch();

		GLSLProgram *createProgram(const std::string &name)
		{
			return m_programManager.createProgram(name);
		}

		bool userControlEnabled() const
		{
			return m_userControlEnabled;
		}
		void enableUserControl(const bool b)
		{
			m_userControlEnabled = b;
		}

	private:
		void mainLoop();

		void handleEvent();
		void handleWindowEvent(const SDL_WindowEvent &evt);
		void handleKeyDownEvent(const SDL_Scancode &code);
		void handleKeyUpEvent(const SDL_Scancode &code);
		void handleMouseButtonDown(const SDL_MouseButtonEvent &button);
		void handleMouseButtonUp(const SDL_MouseButtonEvent &button);
		void handleMouseMotion(const SDL_MouseMotionEvent &motion);
		void handleMouseWheel(const SDL_MouseWheelEvent &wheel);

	private:
		GLSLProgramManager m_programManager;

	private:
		Window m_window;
		UserInterface m_ui;
		Renderer m_renderer;
		double m_timeRendering = 0.f;

		Scene m_scene;

		bool m_running = false;

		bool m_userControlEnabled = true;
		SDL_Event m_evt;
		bool m_leftClick = false;
		float m_camSpeed = 1.f;
		float m_currentSpeed = m_camSpeed;
	};
} // namespace vidocklab

#endif // __APP_HPP__
