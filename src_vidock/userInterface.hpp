/*
 * File: userInterface.hpp
 * Author: Maxime MARIA
 */

#ifndef __USER_INTERFACE_HPP__
#define __USER_INTERFACE_HPP__

#include "imgui/imgui.h"

#include "renderer.hpp"
#include "scene.hpp"
#include "window.hpp"

namespace vidocklab
{
	class UserInterface
	{
	public:
		UserInterface();
		~UserInterface();

		void init(Window *window);
		void display(const Scene &scene, const Renderer &renderer);

	private:
		void showMenuBar();
		void showPerfInfo(bool *open, ImGuiIO &io, const Scene &scene,
						  const Renderer &renderer);
		void showViewerEditor(bool *open);

		void closeAll();

	private:
		Window *m_window = nullptr;
		SDL_Window *m_sdlWindow = nullptr;
		SDL_GLContext m_sdlGLContext = nullptr;

		bool m_showPerformance = true;
		bool m_showMoleculeEditor = false;
		bool m_showViewerEditor = false;
	};
} // namespace vidocklab

#endif // __USER_INTERFACE_HPP__
