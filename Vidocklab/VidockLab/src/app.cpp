/*
 * File: app.cpp (main)
 * Author: Maxime MARIA
 */

#include "app.hpp"

#include <iostream>

#include "GL/gl3w.h"
#include "glm/gtc/type_ptr.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"

#include "GLSLProgramManager.hpp"
#include "chrono.hpp"
#include "exceptions.hpp"
#include "freeflyCamera.hpp"
#include "io/ioUtils.hpp"
#include "io/pdbFile.hpp"

#include "ballAndStickRepDeferred.hpp"
#include "vdwSphereRepGeomShaderDeferred.hpp"

namespace vidocklab
{
	template<>
	App *Singleton<App>::m_instance = nullptr;

	void App::init(int argc, char *argv[])
	{
		AppConfig::exeDirPath = argv[0];
		AppConfig::exeDirPath = AppConfig::exeDirPath.dirPath();
		AppConfig::shadersDirPath
			= AppConfig::exeDirPath + "../../VidockLab/src/shaders/";

		std::cout << AppConfig::exeDirPath << std::endl;

		std::cout << AppConfig::shadersDirPath << std::endl;

		// init SDL window & ui
		m_window.init("VidockLab: here we test!", 1280, 720);
		m_ui.init(&m_window);

		//  init OpenGL
		VerboseOStream::cout << "Initializing OpenGL" << std::endl;
		if (gl3wInit())
		{
			throw GLException("failed to initialize OpenGL");
		}
		if (!gl3wIsSupported(4, 5))
		{
			throw GLException("OpenGL 4.5 not supported");
		}

		VerboseOStream::cout
			<< "--- GL version: " << glGetString(GL_VERSION) << std::endl
			<< "--- GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
			<< std::endl
			<< "--- GL device: " << glGetString(GL_VENDOR) << " "
			<< glGetString(GL_RENDERER) << std::endl;

		m_scene.init(m_window.getWidth(), m_window.getHeight());
	}

	void App::launch()
	{
		m_renderer.init(m_window.getWidth(), m_window.getHeight());
		mainLoop();
	}

	void App::mainLoop()
	{
		m_running = true;
		double t = 0.;
		uint cpt = 0;
		uint idFrame = 0;
		while (m_running)
		{
			if (m_userControlEnabled)
			{
				handleEvent();
			}

			// animation, collision, etc...

			// render scene
			t = m_renderer.render(m_scene) * 1e3;
			m_timeRendering += t;

			// display ui
			m_ui.display(m_scene, m_renderer);

			m_window.swapBuffers();
		}
	}

	void App::handleEvent()
	{
		while (m_window.pollEvent(m_evt))
		{
			ImGui_ImplSDL2_ProcessEvent(&m_evt);

			switch (m_evt.type)
			{
			case SDL_QUIT:
				m_running = false;
				break;
			case SDL_WINDOWEVENT:
				handleWindowEvent(m_evt.window);
				break;
			case SDL_KEYDOWN:
				handleKeyDownEvent(m_evt.key.keysym.scancode);
				break;
			case SDL_KEYUP:
				handleKeyUpEvent(m_evt.key.keysym.scancode);
				break;
			case SDL_MOUSEBUTTONDOWN:
				handleMouseButtonDown(m_evt.button);
				break;
			case SDL_MOUSEBUTTONUP:
				handleMouseButtonUp(m_evt.button);
				break;
			case SDL_MOUSEMOTION:
				handleMouseMotion(m_evt.motion);
				break;
			case SDL_MOUSEWHEEL:
				handleMouseWheel(m_evt.wheel);
			default:
				break;
			}
		}
	}

	void App::handleWindowEvent(const SDL_WindowEvent &evt)
	{
		switch (evt.event)
		{
		case SDL_WINDOWEVENT_CLOSE:
			if (evt.windowID == SDL_GetWindowID(m_window.getSDLWindow()))
			{
				m_running = false;
			}
			break;
		default:
			break;
		}
	}

	void App::handleKeyDownEvent(const SDL_Scancode &code)
	{
		switch (code)
		{
		case SDL_SCANCODE_ESCAPE:
			m_running = false;
			break;
		case SDL_SCANCODE_LSHIFT:
			m_currentSpeed = m_camSpeed * 15.f;
			break;
		case SDL_SCANCODE_LCTRL:
			m_currentSpeed = m_camSpeed / 5.f;
			break;
		// TODO temp frame modifier
		case SDL_SCANCODE_L:
			m_scene.m_currentFrame
				= (m_scene.m_currentFrame + 1) % m_scene.m_molecule->m_nbFrames;
			m_scene.m_molecule->updateFrame(m_scene.m_currentFrame);
			break;
		// camera
		case SDL_SCANCODE_W:
			m_scene.m_camera->moveFront(m_currentSpeed);
			break;
		case SDL_SCANCODE_A:
			m_scene.m_camera->moveLeft(m_currentSpeed);
			break;
		case SDL_SCANCODE_S:
			m_scene.m_camera->moveFront(-m_currentSpeed);
			break;
		case SDL_SCANCODE_D:
			m_scene.m_camera->moveLeft(-m_currentSpeed);
			break;
		case SDL_SCANCODE_R:
			m_scene.m_camera->moveUp(m_currentSpeed);
			break;
		case SDL_SCANCODE_F:
			m_scene.m_camera->moveUp(-m_currentSpeed);
			break;
		case SDL_SCANCODE_Q:
			m_scene.m_camera->rotateLeft(1.f);
			break;
		case SDL_SCANCODE_E:
			m_scene.m_camera->rotateLeft(-1.f);
			break;
		case SDL_SCANCODE_SPACE:
			m_scene.m_camera->printInfo();
			break;
		// shading
		case SDL_SCANCODE_1:
			m_renderer.setShading(Renderer::Shading::LAMBERT);
			break;
		case SDL_SCANCODE_2:
			m_renderer.setShading(Renderer::Shading::BLINN_PHONG);
			break;
		case SDL_SCANCODE_3:
			m_renderer.setShading(Renderer::Shading::TOON);
			break;
		// representations
		/// TODO : beurk beurk beurk
		/*case SDL_SCANCODE_F1:
		{
			if (m_scene.m_moleculeReps[AMoleculeRep::RepType::WIRE] == nullptr)
			{
				m_scene.m_moleculeReps[AMoleculeRep::RepType::WIRE]
					= new WireRep(*(m_scene.m_molecule));
			}
			m_scene.m_moleculeReps[AMoleculeRep::RepType::WIRE]
				->changeVisibility();
			std::cout << "Wire representation: ";
			if (!m_scene.m_moleculeReps[AMoleculeRep::RepType::WIRE]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;
		case SDL_SCANCODE_F2:
		{
			if (m_scene.m_moleculeReps[AMoleculeRep::RepType::VDW_SPHERES]
				== nullptr)
			{
				m_scene.m_moleculeReps[AMoleculeRep::RepType::VDW_SPHERES]
					= new VdwSphereSSBORep(*(m_scene.m_molecule));
			}
			m_scene.m_moleculeReps[AMoleculeRep::RepType::VDW_SPHERES]
				->changeVisibility();
			std::cout << "Sphere representation (SSBO): ";
			if (!m_scene.m_moleculeReps[AMoleculeRep::RepType::VDW_SPHERES]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;
		case SDL_SCANCODE_F3:
		{
			if (m_scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER]
				== nullptr)
			{
				m_scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER]
					= new VdwSphereRepGeomShader(*(m_scene.m_molecule));
			}
			m_scene
				.m_moleculeReps[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER]
				->changeVisibility();
			std::cout << "Sphere representation (geometry shader): ";
			if (!m_scene
					 .m_moleculeReps
						 [AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;*/
		case SDL_SCANCODE_F4:
		{
			if (m_scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER_DEFERRED]
				== nullptr)
			{
				/*m_scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER_DEFERRED]
					= new
				   VdwSphereRepGeomShaderDeferred(*(m_scene.m_molecule));*/
				m_scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER_DEFERRED]
					= new VdwSphereRepGeomShaderDeferred(m_scene.m_molecule);
			}
			m_scene
				.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_GEOM_SHADER_DEFERRED]
				->changeVisibility();
			std::cout << "Sphere representation (deferred shading): ";
			if (!m_scene
					 .m_moleculeReps[AMoleculeRep::RepType::
										 VDW_SPHERES_GEOM_SHADER_DEFERRED]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;
		/*case SDL_SCANCODE_F4:
		{
			if (m_scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_POINT_SPRITES]
				== nullptr)
			{
				m_scene.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_POINT_SPRITES]
					= new VdwSpherePointSpritesRep(*(m_scene.m_molecule));
			}
			m_scene
				.m_moleculeReps
					[AMoleculeRep::RepType::VDW_SPHERES_POINT_SPRITES]
				->changeVisibility();
			std::cout << "Sphere representation (point sprites): ";
			if (!m_scene
					 .m_moleculeReps
						 [AMoleculeRep::RepType::VDW_SPHERES_POINT_SPRITES]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;*/
		case SDL_SCANCODE_F5:
		{
			if (m_scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER_DEFERRED]
				== nullptr)
			{
				m_scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER_DEFERRED]
					= new BallAndStickRepDeferred(m_scene.m_molecule);
			}
			m_scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER_DEFERRED]
				->changeVisibility();
			std::cout << "Only cylinder representation (deferred SHADING): ";
			if (!m_scene
					 .m_moleculeReps[AMoleculeRep::RepType::CYLINDER_DEFERRED]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;
		/*case SDL_SCANCODE_F5:
		{
			if (m_scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER]
				== nullptr)
			{
				m_scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER]
					= new BallAndStickRep(*(m_scene.m_molecule));
			}
			m_scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER]
				->changeVisibility();
			std::cout << "Only cylinder representation (geometry shader): ";
			if (!m_scene.m_moleculeReps[AMoleculeRep::RepType::CYLINDER]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;*/
		/*case SDL_SCANCODE_B:
		{
			if (m_scene.m_moleculeReps[AMoleculeRep::RepType::BBOX] == nullptr)
			{
				m_scene.m_moleculeReps[AMoleculeRep::RepType::BBOX]
					= new BboxRep(*(m_scene.m_molecule));
			}
			m_scene.m_moleculeReps[AMoleculeRep::RepType::BBOX]
				->changeVisibility();
			std::cout << "BBox: ";
			if (!m_scene.m_moleculeReps[AMoleculeRep::RepType::BBOX]
					 ->isVisible())
				std::cout << "not ";
			std::cout << "visible" << std::endl;
		}
		break;*/
		default:
			break;
		}
	}

	void App::handleKeyUpEvent(const SDL_Scancode &code)
	{
		switch (code)
		{
		case SDL_SCANCODE_LSHIFT:
		case SDL_SCANCODE_LCTRL:
			m_currentSpeed = m_camSpeed;
			break;
		default:
			break;
		}
	}

	void App::handleMouseButtonDown(const SDL_MouseButtonEvent &button)
	{
		switch (button.button)
		{
		case SDL_BUTTON_LEFT:
			m_leftClick = true;
		default:
			break;
		}
	}

	void App::handleMouseButtonUp(const SDL_MouseButtonEvent &button)
	{
		switch (button.button)
		{
		case SDL_BUTTON_LEFT:
			m_leftClick = false;
		default:
			break;
		}
	}

	void App::handleMouseMotion(const SDL_MouseMotionEvent &motion)
	{
		if (m_leftClick)
		{
			m_scene.m_camera->rotateLeft(0.1f * motion.xrel);
			m_scene.m_camera->rotateUp(0.1f * motion.yrel);
		}
	}

	void App::handleMouseWheel(const SDL_MouseWheelEvent &wheel) {}
} // namespace vidocklab
