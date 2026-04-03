#ifndef __VTX_RENDERER_BENCH_INPUT_MANAGER__
#define __VTX_RENDERER_BENCH_INPUT_MANAGER__

#include <SDL3/SDL.h>
#include <optional>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Bench
{
	struct WindowSize
	{
		size_t width  = 0;
		size_t height = 0;
	};

	struct MousePick
	{
		uint x = 0;
		uint y = 0;
	};

	struct FrameInputs
	{
		bool					  closeRequested   = false;
		bool					  restoreRequested = false;
		std::optional<WindowSize> resize		   = std::nullopt;
		std::optional<MousePick>  mousePick		   = std::nullopt;
		Vec3i					  translation	   = VEC3I_ZERO;
		Vec2i					  rotation		   = VEC2I_ZERO;
		int						  zoom			   = 0;
		std::vector<SDL_Scancode> pressedKeys;
		std::vector<FilePath>	  droppedFiles;
	};

	class InputManager
	{
	  public:
		inline bool isKeyPressed( const SDL_Scancode p_key ) const { return _keys[ p_key ]; }
		inline bool isMouseButtonPressed( const size_t p_button ) const { return _mouseButtons[ p_button ]; }

		inline void handle( const SDL_Event & p_event )
		{
			switch ( p_event.type )
			{
			case SDL_EVENT_QUIT: _frame.closeRequested = true; break;
			case SDL_EVENT_KEY_DOWN:
				if ( not _keys[ p_event.key.scancode ] )
				{
					_frame.pressedKeys.push_back( p_event.key.scancode );
				}
				_keys[ p_event.key.scancode ] = true;
				break;
			case SDL_EVENT_KEY_UP: _keys[ p_event.key.scancode ] = false; break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				_mouseButtons[ p_event.button.button - 1 ] = true;
				if ( p_event.button.button == SDL_BUTTON_LEFT )
					_frame.mousePick = MousePick { uint( p_event.button.x ), uint( p_event.button.y ) };
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP: _mouseButtons[ p_event.button.button - 1 ] = false; break;
			case SDL_EVENT_MOUSE_MOTION:
				if ( _mouseButtons[ 2 ] )
				{
					_deltaMouse.x += int( p_event.motion.xrel );
					_deltaMouse.y += int( p_event.motion.yrel );
				}
				break;
			case SDL_EVENT_MOUSE_WHEEL: _deltaWheel -= int( p_event.wheel.y ); break;
			case SDL_EVENT_WINDOW_RESIZED:
				_frame.resize = WindowSize { size_t( p_event.window.data1 ), size_t( p_event.window.data2 ) };
				break;
			case SDL_EVENT_WINDOW_RESTORED: _frame.restoreRequested = true; break;
			case SDL_EVENT_DROP_FILE: _frame.droppedFiles.emplace_back( p_event.drop.data ); break;

			default: break;
			}
		}

		inline void update()
		{
			if ( _keys[ SDL_SCANCODE_W ] )
			{
				_deltaMoveInputs.z--;
			}
			if ( _keys[ SDL_SCANCODE_S ] )
			{
				_deltaMoveInputs.z++;
			}
			if ( _keys[ SDL_SCANCODE_A ] )
			{
				_deltaMoveInputs.x--;
			}
			if ( _keys[ SDL_SCANCODE_D ] )
			{
				_deltaMoveInputs.x++;
			}
			if ( _keys[ SDL_SCANCODE_R ] )
			{
				_deltaMoveInputs.y++;
			}
			if ( _keys[ SDL_SCANCODE_F ] )
			{
				_deltaMoveInputs.y--;
			}
			if ( _keys[ SDL_SCANCODE_ESCAPE ] )
			{
				_frame.closeRequested = true;
			}
		}

		inline FrameInputs consumeInputs()
		{
			_frame.translation = _deltaMoveInputs;
			_frame.rotation	   = _deltaMouse;
			_frame.zoom		   = _deltaWheel;

			FrameInputs result = std::move( _frame );
			_frame			   = FrameInputs {};

			_deltaMoveInputs = { 0, 0, 0 };
			_deltaMouse		 = { 0, 0 };
			_deltaWheel		 = 0;

			return result;
		}

	  private:
		bool _keys[ SDL_SCANCODE_COUNT ] = { false };
		bool _mouseButtons[ 3 ]			 = { false };

		FrameInputs _frame;
		Vec3i		_deltaMoveInputs = { 0, 0, 0 };
		Vec2i		_deltaMouse		 = { 0, 0 };
		int			_deltaWheel		 = 0;
	};
} // namespace VTX::Bench

#endif
