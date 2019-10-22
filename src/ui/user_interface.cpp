#include "user_interface.hpp"

namespace VTX
{
	namespace UI
	{
		UserInterface::UserInterface()
		{
			if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ) != 0 )
			{
				// throw SDLException( SDL_GetError() );
			}

			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, 0 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK,
								 SDL_GL_CONTEXT_PROFILE_CORE );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 5 );

			SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
			SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
			SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

			// SDL_GetCurrentDisplayMode( 0, &m_displayMode );

			_window = SDL_CreateWindow( "Test",
										SDL_WINDOWPOS_UNDEFINED,
										SDL_WINDOWPOS_UNDEFINED,
										1920,
										1080,
										SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
											| SDL_WINDOW_RESIZABLE
											| SDL_WINDOW_ALLOW_HIGHDPI );

			if ( _window == nullptr )
			{
				// throw SDLException( SDL_GetError() );
			}

			_glContext = SDL_GL_CreateContext( _window );
			if ( _glContext == nullptr )
			{
				// throw SDLException( SDL_GetError() );
			}

			SDL_GL_SetSwapInterval( true );
		}

		UserInterface::~UserInterface()
		{
			if ( _glContext ) { SDL_GL_DeleteContext( _glContext ); }
			if ( _window ) { SDL_DestroyWindow( _window ); }
			SDL_Quit();
		}

	} // namespace UI
} // namespace VTX
