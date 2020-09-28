#ifndef __VTX_USER_INTERFACE__
#define __VTX_USER_INTERFACE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component.hpp"
#include "ui/main_window.hpp"
#include <GL/gl3w.h>
#include <SDL.h>

namespace VTX
{
	namespace UI
	{
		class UserInterface : public BaseComponent
		{
		  public:
			UserInterface();
			~UserInterface();
			void print() const;
			bool getEvent( SDL_Event & evt ) const;
			void setTheme() const;
			void setVSync( const bool );

			virtual const std::string & getName() const override { return ID::UI::USER_INTERFACE; }

			inline const ID::VTX_ID getFocusedWindow() const { return _focusedWindow; }
			inline void				setFocusedWindow( const ID::VTX_ID & p_id ) { _focusedWindow = p_id; }
			inline const ID::VTX_ID getHoveredWindow() const { return _hoveredWindow; }
			inline void				setHoveredWindow( const ID::VTX_ID & p_id ) { _hoveredWindow = p_id; }

			// Qt.
			inline const UI::MainWindow & getMainWindow() const { return *_mainWindow; }
			inline UI::MainWindow &		  getMainWindow() { return *_mainWindow; }

		  protected:
			virtual bool _drawHeader() override;
			virtual void _drawContent() override;
			virtual void _drawFooter() override;
			virtual void _draw() override;

		  private:
			// IMGUI
			SDL_Window *	_window	   = nullptr;
			SDL_GLContext	_glContext = nullptr;
			SDL_DisplayMode _displayMode;

			ID::VTX_ID _focusedWindow = ID::UI::USER_INTERFACE;
			ID::VTX_ID _hoveredWindow = ID::UI::USER_INTERFACE;

			void _initSDL2();
			void _initGL();
			void _initIMGUI();

			void _disposeAll();
			void _disposeSDL2();
			void _disposeGL();
			void _disposeIMGUI();

			// Qt.
			UI::MainWindow * _mainWindow = nullptr;
		};
	} // namespace UI
} // namespace VTX

#endif
