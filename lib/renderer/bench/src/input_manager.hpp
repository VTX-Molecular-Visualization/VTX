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
		inline void addCallbackClose( const Util::Callback<>::Func & p_cb ) { _callbackClose += p_cb; }
		inline void addCallbackResize( const Util::Callback<size_t, size_t>::Func & p_cb ) { _callbackResize += p_cb; }
		inline void addCallbackTranslate( const Util::Callback<Vec3i>::Func & p_cb ) { _callbackTranslate += p_cb; }
		inline void addCallbackRotate( const Util::Callback<Vec2i>::Func & p_cb ) { _callbackRotate += p_cb; }
		inline void addCallbackZoom( const Util::Callback<int>::Func & p_cb ) { _callbackZoom += p_cb; }
		inline void addCallbackMouseMotion( const Util::Callback<Vec2i>::Func & p_cb ) { _callbackMouseMotion += p_cb; }
		inline void addCallbackRestore( const Util::Callback<>::Func & p_cb ) { _callbackRestore += p_cb; }
		inline void addCallbackMousePick( const Util::Callback<size_t, size_t>::Func & p_cb )
		{
			_callbackMousePick += p_cb;
		}

		inline bool isKeyPressed( const SDL_Scancode p_key ) const { return _keys[ p_key ]; }
		inline bool isMouseButtonPressed( const size_t p_button ) const { return _mouseButtons[ p_button ]; }

		inline void handle( const SDL_Event & p_event )
		{
			switch ( p_event.type )
			{
			case SDL_QUIT: _onClose(); break;
			case SDL_KEYDOWN: _keys[ p_event.key.keysym.scancode ] = true; break;
			case SDL_KEYUP: _keys[ p_event.key.keysym.scancode ] = false; break;
			case SDL_MOUSEBUTTONDOWN:
				_mouseButtons[ p_event.button.button - 1 ] = true;
				if ( p_event.button.button == SDL_BUTTON_LEFT )
					_onMousePick( p_event.button.x, p_event.button.y );
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
				_onMouseMotion( x, y );
				break;
			case SDL_MOUSEWHEEL: _deltaWheel += p_event.wheel.y; break;
			case SDL_WINDOWEVENT:
				switch ( p_event.window.event )
				{
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				case SDL_WINDOWEVENT_RESIZED: _onResize( p_event.window.data1, p_event.window.data2 ); break;
				case SDL_WINDOWEVENT_RESTORED: _onRestore(); break;
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
				_onClose();
			}
		}

		inline void consumeInputs()
		{
			if ( _deltaMoveInputs != VEC3I_ZERO )
			{
				_callbackTranslate( _deltaMoveInputs );
			}

			if ( _deltaMouse != VEC2I_ZERO )
			{
				_callbackRotate( _deltaMouse );
			}

			if ( _deltaWheel != 0 )
			{
				_callbackZoom( _deltaWheel );
			}

			_deltaMoveInputs = { 0, 0, 0 };
			_deltaMouse		 = { 0, 0 };
			_deltaWheel		 = 0;
		}

	  private:
		bool _keys[ SDL_NUM_SCANCODES ] = { false };
		bool _mouseButtons[ 3 ]			= { false };

		Vec3i _deltaMoveInputs = { 0, 0, 0 };
		Vec2i _deltaMouse	   = { 0, 0 };
		int	  _deltaWheel	   = 0;

		Util::Callback<>			   _callbackClose;
		Util::Callback<size_t, size_t> _callbackResize;
		Util::Callback<Vec3i>		   _callbackTranslate;
		Util::Callback<Vec2i>		   _callbackRotate;
		Util::Callback<int>			   _callbackZoom;
		Util::Callback<Vec2i>		   _callbackMouseMotion;
		Util::Callback<>			   _callbackRestore;
		Util::Callback<size_t, size_t> _callbackMousePick;

		inline void _onClose() { _callbackClose(); }
		inline void _onResize( const size_t p_w, const size_t p_h ) { _callbackResize( p_w, p_h ); }
		inline void _onMouseMotion( const size_t p_x, const size_t p_y ) { _callbackMouseMotion( { p_x, p_y } ); }
		inline void _onRestore() { _callbackRestore(); }
		inline void _onMousePick( const size_t p_x, const size_t p_y ) { _callbackMousePick( p_x, p_y ); }
	};
} // namespace VTX::Bench

#endif
