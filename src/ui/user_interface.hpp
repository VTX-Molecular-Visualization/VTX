#ifndef __USER_INTERFACE__
#define __USER_INTERFACE__

#include "base_component.hpp"
#include <SDL2/SDL.h>

namespace VTX
{
	namespace UI
	{
		class UserInterface : public BaseComponent
		{
		  public:
			UserInterface();
			~UserInterface();

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
			SDL_Window *  _window	 = nullptr;
			SDL_GLContext _glContext = nullptr;
			// SDL_DisplayMode _displayMode;

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
