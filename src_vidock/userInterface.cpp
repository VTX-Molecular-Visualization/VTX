/*
 * File: userInterface.cpp
 * Author: Maxime MARIA
 */

#include "userInterface.hpp"

#include <iostream>

#include "GL/gl3w.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

#include "app.hpp"
#include "appConfig.hpp"

namespace vidocklab
{
	UserInterface::UserInterface() {}

	UserInterface::~UserInterface()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		if (ImGui::GetCurrentContext() != nullptr)
			ImGui::DestroyContext();
	}

	void UserInterface::init(Window *window)
	{
		m_window = window;
		m_sdlWindow = m_window->getSDLWindow();
		m_sdlGLContext = m_window->getGLContext();

		if (!IMGUI_CHECKVERSION())
			throw std::exception("imgui check version error");

		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		// Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

		// dark style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplSDL2_InitForOpenGL(m_sdlWindow, m_sdlGLContext);
		ImGui_ImplOpenGL3_Init(AppConfig::glslVersion.c_str());
	}

	void UserInterface::display(const Scene &scene, const Renderer &renderer)
	{
		App &app = App::get();
		ImGuiIO &io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// TODO if we want gamepad control
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(m_sdlWindow);
		ImGui::NewFrame();

		showMenuBar();

		if (m_showPerformance)
			showPerfInfo(&m_showPerformance, io, scene, renderer);
		if (m_showViewerEditor)
			showViewerEditor(&m_showViewerEditor);

		ImGui::Render();
		SDL_GL_MakeCurrent(m_sdlWindow, m_sdlGLContext);
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void UserInterface::showMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New"))
				{
					std::cout << "New menu" << std::endl;
				}
				if (ImGui::MenuItem("Open"))
				{
					std::cout << "Open menu" << std::endl;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				if (ImGui::MenuItem("Viewer Editor"))
				{
					m_showViewerEditor = !m_showViewerEditor;
				}
				if (ImGui::MenuItem("Performance Data"))
				{
					m_showPerformance = !m_showPerformance;
				}
				ImGui::EndMenu();
			}
			ImGui::Separator();
			ImGui::Separator();
			if (ImGui::MenuItem("Close All"))
			{
				closeAll();
			}
		}
		ImGui::EndMainMenuBar();
	}

	void UserInterface::showPerfInfo(bool *open, ImGuiIO &io,
									 const Scene &scene,
									 const Renderer &renderer)
	{
		ImGui::Begin("Performance Data", open);
		ImGui::Text("Display size: %dx%d", int(io.DisplaySize.x),
					int(io.DisplaySize.y));
		ImGui::Text("Framerate: %.3f ms/frame (%.1f FPS)", 1e3f / io.Framerate,
					io.Framerate);
		ImGui::Text("Molecule: %s", scene.m_molecule->m_name.c_str());
		ImGui::Text(" - %d atoms", scene.m_molecule->m_nbAtoms);
		ImGui::Text(" - %d bonds", scene.m_molecule->m_nbBonds);

		ImGui::Text("Frame %d/%d", scene.m_currentFrame + 1,
					scene.m_molecule->m_nbFrames);
		ImGui::SliderInt("Image number", (int *)(&scene.m_currentFrame), 1,
						 scene.m_molecule->m_nbFrames);
		if (ImGui::IsItemEdited())
		{
			scene.m_molecule->updateFrame(scene.m_currentFrame - 1);
		}

		ImGui::End();
	}

	void UserInterface::showViewerEditor(bool *open)
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Viewer Editor", open);
		if (ImGui::BeginTabBar("MyTabBar"))
		{
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	void UserInterface::closeAll()
	{
		m_showPerformance = false;
		m_showViewerEditor = false;
		m_showMoleculeEditor = false;
	}

} // namespace vidocklab
