#ifndef __VTX_USER_INTERFACE__
#define __VTX_USER_INTERFACE__

#include "base_component.hpp"
#include <SDL2/SDL.h>

namespace VTX
{
	namespace UI
	{
		enum THEME
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
			void setTheme() const;

			virtual COMPONENT_NAME getName() const override { return COMPONENT_NAME::UI; }

		  protected:
			virtual void _addComponents() override;
			virtual void _draw() override;

		  private:
			SDL_Window *  _window	 = nullptr;
			SDL_GLContext _glContext = nullptr;
			// SDL_DisplayMode _displayMode;

			bool _showMenu		= true;
			bool _showConsole	= true;
			bool _showScene		= true;
			bool _showSelection = true;

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
