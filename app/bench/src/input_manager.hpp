#ifndef __VTX_RENDERER_BENCH_INPUT_MANAGER__
#define __VTX_RENDERER_BENCH_INPUT_MANAGER__

#include <SDL3/SDL.h>
#include <functional>
#include <util/callback.hpp>
#include <util/constants.hpp>
#include <util/logger.hpp>
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
			case SDL_EVENT_QUIT: onClose(); break;
			case SDL_EVENT_KEY_DOWN:
				_keys[ p_event.key.scancode ] = true;
				onKeyPressed( p_event.key.scancode );
				break;
			case SDL_EVENT_KEY_UP: _keys[ p_event.key.scancode ] = false; break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				_mouseButtons[ p_event.button.button - 1 ] = true;
				if ( p_event.button.button == SDL_BUTTON_LEFT )
					onMousePick( uint( p_event.button.x ), uint( p_event.button.y ) );
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP: _mouseButtons[ p_event.button.button - 1 ] = false; break;
			case SDL_EVENT_MOUSE_MOTION:
				if ( _mouseButtons[ 2 ] )
				{
					_deltaMouse.x += int( p_event.motion.xrel );
					_deltaMouse.y += int( p_event.motion.yrel );
				}
				float x, y;
				SDL_GetMouseState( &x, &y );
				onMouseMotion( { x, y } );
				break;
			case SDL_EVENT_MOUSE_WHEEL: _deltaWheel += int( p_event.wheel.y ); break;
			case SDL_EVENT_WINDOW_RESIZED: onResize( p_event.window.data1, p_event.window.data2 ); break;
			case SDL_EVENT_WINDOW_RESTORED: onRestore(); break;
			case SDL_EVENT_DROP_FILE:
				onFileDrop( p_event.drop.data );
				// SDL_free( p_event.drop.data );
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
				onClose();
			}
		}

		inline void consumeInputs()
		{
			if ( _deltaMoveInputs != VEC3I_ZERO )
			{
				onTranslate( _deltaMoveInputs );
			}

			if ( _deltaMouse != VEC2I_ZERO )
			{
				onRotate( _deltaMouse );
			}

			if ( _deltaWheel != 0 )
			{
				onZoom( _deltaWheel );
			}

			_deltaMoveInputs = { 0, 0, 0 };
			_deltaMouse		 = { 0, 0 };
			_deltaWheel		 = 0;
		}

		Util::Callback<>			   onClose;
		Util::Callback<size_t, size_t> onResize;
		Util::Callback<Vec3i &>		   onTranslate;
		Util::Callback<Vec2i &>		   onRotate;
		Util::Callback<int>			   onZoom;
		Util::Callback<const Vec2i &>  onMouseMotion;
		Util::Callback<>			   onRestore;
		Util::Callback<uint, uint>	   onMousePick;
		Util::Callback<SDL_Scancode>   onKeyPressed;
		Util::Callback<FilePath>	   onFileDrop;

	  private:
		bool _keys[ SDL_SCANCODE_COUNT ] = { false };
		bool _mouseButtons[ 3 ]			 = { false };

		Vec3i _deltaMoveInputs = { 0, 0, 0 };
		Vec2i _deltaMouse	   = { 0, 0 };
		int	  _deltaWheel	   = 0;
	};
} // namespace VTX::Bench

#endif
