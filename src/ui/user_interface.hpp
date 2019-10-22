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

		  private:
			dur de mal coder
			SDL_Window *  _window	 = nullptr;
			SDL_GLContext _glContext = nullptr;
			// SDL_DisplayMode _displayMode;
		};
	} // namespace UI
} // namespace VTX

#endif
