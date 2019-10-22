/*
 * File: window.cpp
 * Author: Maxime MARIA
 */

#include "window.hpp"

#include "appConfig.hpp"
#include "defines.hpp"
#include "exceptions.hpp"
#include "io/ioUtils.hpp"

namespace vidocklab
{
	Window::~Window()
	{
		if (m_glContext)
			SDL_GL_DeleteContext(m_glContext);
		if (m_window)
			SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	void Window::init(const std::string &title, const int width,
					  const int height)
	{
		VerboseOStream::cout << "Initializing window" << std::endl;
		m_title = title;
		m_width = width;
		m_height = height;

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
			throw SDLException(SDL_GetError());

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
							SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		SDL_GetCurrentDisplayMode(0, &m_displayMode);

		VerboseOStream::cout << "--- Create SDL window" << std::endl;
		m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED, m_width, m_height,
									SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
										| SDL_WINDOW_RESIZABLE
										| SDL_WINDOW_ALLOW_HIGHDPI);

		if (m_window == nullptr)
			throw SDLException(SDL_GetError());

		VerboseOStream::cout << "--- Create SDL GL context" << std::endl;
		m_glContext = SDL_GL_CreateContext(m_window);

		if (m_glContext == nullptr)
			throw SDLException(SDL_GetError());

		SDL_GL_SetSwapInterval(AppConfig::vSync); // enable vsync

		m_idWindow = SDL_GetWindowID(m_window);

		VerboseOStream::cout << DONE_STRING << std::endl;
	}

} // namespace vidocklab
