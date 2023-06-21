#ifndef __VTX_RENDERER_BENCH_INPUT_MANAGER__
#define __VTX_RENDERER_BENCH_INPUT_MANAGER__

#include <SDL.h>
#include <functional>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::Bench
{
	class InputManager
	{
	  public:
		using CallbackClose		= std::function<void()>;
		using CallbackResize	= std::function<void( const size_t, const size_t )>;
		using CallbackTranslate = std::function<void( const Vec3i & )>;
		using CallbackRotate	= std::function<void( const Vec2i & )>;

		inline void setCallbackClose( const CallbackClose & p_cb ) { _callbackClose = p_cb; }
		inline void setCallbackResize( const CallbackResize & p_cb ) { _callbackResize = p_cb; }
		inline void setCallbackTranslate( const CallbackTranslate & p_cb ) { _callbackTranslate = p_cb; }
		inline void setCallbackRotate( const CallbackRotate & p_cb ) { _callbackRotate = p_cb; }

		inline bool isKeyPressed( const SDL_Scancode p_key ) const { return _keys[ p_key ]; }
		inline bool isMouseButtonPressed( const size_t p_button ) const { return _mouseButtons[ p_button ]; }

		inline void handle( const SDL_Event & p_event )
		{
			switch ( p_event.type )
			{
			case SDL_QUIT: _onClose(); break;
			case SDL_KEYDOWN: _keys[ p_event.key.keysym.scancode ] = true; break;
			case SDL_KEYUP: _keys[ p_event.key.keysym.scancode ] = false; break;
			case SDL_MOUSEBUTTONDOWN: _mouseButtons[ p_event.button.button - 1 ] = true; break;
			case SDL_MOUSEBUTTONUP: _mouseButtons[ p_event.button.button - 1 ] = false; break;
			case SDL_MOUSEMOTION:
				if ( _mouseButtons[ 0 ] )
				{
					_deltaMouse.x += p_event.motion.xrel;
					_deltaMouse.y += p_event.motion.yrel;
				}
				break;
			case SDL_WINDOWEVENT:
				switch ( p_event.window.event )
				{
				case SDL_WINDOWEVENT_RESIZED: _onResize( p_event.window.data1, p_event.window.data2 ); break;
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
			if ( _deltaMoveInputs != VEC3I_ZERO && _callbackTranslate )
			{
				_callbackTranslate( _deltaMoveInputs );
			}

			if ( _deltaMouse != VEC2I_ZERO && _callbackRotate )
			{
				_callbackRotate( _deltaMouse );
			}

			_deltaMoveInputs = { 0, 0, 0 };
			_deltaMouse		 = { 0, 0 };
		}

	  private:
		bool _keys[ SDL_NUM_SCANCODES ] = { false };
		bool _mouseButtons[ 3 ]			= { false };

		Vec3i _deltaMoveInputs = { 0, 0, 0 };
		Vec2i _deltaMouse	   = { 0, 0 };

		CallbackClose	  _callbackClose;
		CallbackResize	  _callbackResize;
		CallbackTranslate _callbackTranslate;
		CallbackRotate	  _callbackRotate;

		inline void _onClose()
		{
			if ( _callbackClose )
			{
				_callbackClose();
			}
		}

		inline void _onResize( const size_t p_w, const size_t p_h )
		{
			if ( _callbackResize )
			{
				_callbackResize( p_w, p_h );
			}
		}
	};
} // namespace VTX::Bench

#endif
