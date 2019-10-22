/*
 * File: window.hpp
 * Author: Maxime MARIA
 */

#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <string>

#include "SDL.h"

namespace vidocklab
{
	class Window
	{
	public:
		Window() = default;
		~Window();

		void init(const std::string &, const int width, const int height);

		SDL_Window *getSDLWindow() const
		{
			return m_window;
		}

		SDL_GLContext getGLContext() const
		{
			return m_glContext;
		}

		bool pollEvent(SDL_Event &evt) const
		{
			return SDL_PollEvent(&evt);
		}

		void swapBuffers() const
		{
			SDL_GL_SwapWindow(m_window);
		}

		int getWidth() const
		{
			return m_width;
		}

		int getHeight() const
		{
			return m_height;
		}

	private:
		std::string m_title = "Untitled";
		int m_width = 1280;
		int m_height = 720;

		SDL_Window *m_window = nullptr;
		SDL_GLContext m_glContext = nullptr;
		SDL_DisplayMode m_displayMode;
		int m_idWindow = -1;
	};
} // namespace vidocklab

#endif // __WINDOW_HPP__
