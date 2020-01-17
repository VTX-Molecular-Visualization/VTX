#ifndef __VTX_USER_INTERFACE__
#define __VTX_USER_INTERFACE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include <GL/gl3w.h>
#include <SDL2/SDL.h>

// GL error callback return type.
typedef void( APIENTRY * DEBUGPROC )( GLenum		 p_source,
									  GLenum		 p_type,
									  GLuint		 p_id,
									  GLenum		 p_severity,
									  GLsizei		 p_length,
									  const GLchar * p_message,
									  const void *	 p_userParam );

namespace VTX
{
	namespace UI
	{
		enum class THEME : int
		{
			LIGHT,
			DARK,
			CLASSIC
		};

		class UserInterface : public BaseComponent
		{
		  public:
			UserInterface();
			~UserInterface();
			void printInfos() const;
			bool pollEvent( SDL_Event & evt ) const;
			void setTheme() const;

			virtual COMPONENT_NAME getName() const override { return COMPONENT_NAME::UI; }

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
			SDL_Window *	_window	   = nullptr;
			SDL_GLContext	_glContext = nullptr;
			SDL_DisplayMode _displayMode;

			bool _showMenu		   = true;
			bool _showConsole	   = true;
			bool _showScene		   = true;
			bool _showInspector	   = true;
			bool _showCameraEditor = true;

			void _initSDL2();
			void _initGL();
			void _initIMGUI();

			void _disposeAll();
			void _disposeSDL2();
			void _disposeGL();
			void _disposeIMGUI();
		};
	} // namespace UI
} // namespace VTX

#endif
