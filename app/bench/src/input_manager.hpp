#ifndef __VTX_RENDERER_BENCH_INPUT_MANAGER__
#define __VTX_RENDERER_BENCH_INPUT_MANAGER__

#include <SDL.h>
#include <functional>
#include <util/callback.hpp>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	class InputManager
	{
	  public:
		inline bool isKeyPressed( const SDL_Scancode p_key ) const { return _keys[ p_key ]; }
		inline bool isMouseButtonPressed( const size_t p_button ) const { return _mouseButtons[ p_button ]; }

		inline void handle( const SDL_Event & p_event )
		{
			switch ( p_event.type )
			{
			case SDL_QUIT: callbackClose(); break;
			case SDL_KEYDOWN:
				_keys[ p_event.key.keysym.scancode ] = true;
				callbackKeyPressed( p_event.key.keysym.scancode );
				break;
			case SDL_KEYUP: _keys[ p_event.key.keysym.scancode ] = false; break;
			case SDL_MOUSEBUTTONDOWN:
				_mouseButtons[ p_event.button.button - 1 ] = true;
				if ( p_event.button.button == SDL_BUTTON_LEFT )
					callbackMousePick( p_event.button.x, p_event.button.y );
				break;
			case SDL_MOUSEBUTTONUP: _mouseButtons[ p_event.button.button - 1 ] = false; break;
			case SDL_MOUSEMOTION:
				if ( _mouseButtons[ 2 ] )
				{
					_deltaMouse.x += p_event.motion.xrel;
					_deltaMouse.y += p_event.motion.yrel;
				}
				int x, y;
				SDL_GetMouseState( &x, &y );
				callbackMouseMotion( { x, y } );
				break;
			case SDL_MOUSEWHEEL: _deltaWheel += p_event.wheel.y; break;
			case SDL_WINDOWEVENT:
				switch ( p_event.window.event )
				{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				case SDL_WINDOWEVENT_RESIZED: callbackResize( p_event.window.data1, p_event.window.data2 ); break;
				case SDL_WINDOWEVENT_RESTORED: callbackRestore(); break;
				}
				break;
			default: break;
			}
		}

		inline void update()
		{
			if ( _keys[ SDL_SCANCODE_W ] )
			{
				_deltaMoveInputs.z++;
			}
			if ( _keys[ SDL_SCANCODE_S ] )
			{
				_deltaMoveInputs.z--;
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
				callbackClose();
			}
		}

		inline void consumeInputs()
		{
			if ( _deltaMoveInputs != VEC3I_ZERO )
			{
				callbackTranslate( _deltaMoveInputs );
			}

			if ( _deltaMouse != VEC2I_ZERO )
			{
				callbackRotate( _deltaMouse );
			}

			if ( _deltaWheel != 0 )
			{
				callbackZoom( _deltaWheel );
			}

			_deltaMoveInputs = { 0, 0, 0 };
			_deltaMouse		 = { 0, 0 };
			_deltaWheel		 = 0;
		}

		Util::Callback<>			   callbackClose;
		Util::Callback<size_t, size_t> callbackResize;
		Util::Callback<Vec3i>		   callbackTranslate;
		Util::Callback<Vec2i>		   callbackRotate;
		Util::Callback<int>			   callbackZoom;
		Util::Callback<Vec2i>		   callbackMouseMotion;
		Util::Callback<>			   callbackRestore;
		Util::Callback<size_t, size_t> callbackMousePick;
		Util::Callback<SDL_Scancode>   callbackKeyPressed;

	  private:
		bool _keys[ SDL_NUM_SCANCODES ] = { false };
		bool _mouseButtons[ 3 ]			= { false };

		Vec3i _deltaMoveInputs = { 0, 0, 0 };
		Vec2i _deltaMouse	   = { 0, 0 };
		int	  _deltaWheel	   = 0;
	};
} // namespace VTX::Bench

#endif
