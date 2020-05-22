#ifndef __VTX_USER_INTERFACE__
#define __VTX_USER_INTERFACE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "generic/base_printable.hpp"
#include "util/logger.hpp"
#include <GL/gl3w.h>
#include <SDL.h>

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
		class UserInterface : public BaseComponent, public Generic::BasePrintable
		{
		  public:
			UserInterface();
			~UserInterface();
			virtual void print() const override;
			bool		 getEvent( SDL_Event & evt ) const;
			void		 setTheme() const;
			void		 setVSync( const bool );

			virtual const std::string & getName() const override { return ID::UI::USER_INTERFACE; }

			inline const ID::VTX_ID getCurrentWindow() const { return _currentWindow; }
			inline void				setCurrentWindow( const ID::VTX_ID & p_id )
			{
				if ( p_id != _currentWindow )
				{
					VTX_DEBUG( "New focus: " + p_id );
					_currentWindow = p_id;
				}
			}

		  protected:
			virtual void _addItems() override;

			virtual bool _drawHeader() override;
			virtual void _drawContent() override;
			virtual void _drawFooter() override;
			virtual void _draw() override;

		  private:
			SDL_Window *	_window	   = nullptr;
			SDL_GLContext	_glContext = nullptr;
			SDL_DisplayMode _displayMode;

			ID::VTX_ID _currentWindow = ID::UI::USER_INTERFACE;

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
