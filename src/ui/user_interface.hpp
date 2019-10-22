#ifndef __USER_INTERFACE__
#define __USER_INTERFACE__

#include <SDL2/SDL.h>

namespace VTX
{
	namespace UI
	{
		class UserInterface
		{
		  public:
			UserInterface();
			~UserInterface();

			void display();

		  private:
			SDL_Window *  _window	 = nullptr;
			SDL_GLContext _glContext = nullptr;
			// SDL_DisplayMode _displayMode;

			void _initSDL2();
			void _initIMGUI();
			void _disposeSDL2();
			void _disposeIMGUI();
			void _showMenuBar();
		};
	} // namespace UI
} // namespace VTX

#endif
