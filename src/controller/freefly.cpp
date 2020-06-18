#include "freefly.hpp"

namespace VTX
{
	namespace Controller
	{
		void Freefly::update( const double p_deltaTime )
		{
			// Rotation.
			if ( _mouseLeftPressed )
			{
				_camera.rotate( Vec3d( -VTX_SETTING().rotationSpeed * (float)_deltaMousePosition.y
										   * ( VTX_SETTING().yAxisInverted ? -1.f : 1.f ),
									   -VTX_SETTING().rotationSpeed * (float)_deltaMousePosition.x,

									   0.f ) );
				_deltaMousePosition.x = 0;
				_deltaMousePosition.y = 0;
			}
			if ( _mouseRightPressed )
			{
				_camera.rotateRoll( VTX_SETTING().rotationSpeed * (float)_deltaMousePosition.x );
				_deltaMousePosition.x = 0;
			}

			// Translation.
			Vec3f translation = VEC3F_ZERO;

			if ( _isKeyPressed( SDL_SCANCODE_W ) || _isKeyPressed( SDL_SCANCODE_UP ) )
			{
				translation.z++;
			}
			if ( _isKeyPressed( SDL_SCANCODE_S ) || _isKeyPressed( SDL_SCANCODE_DOWN ) )
			{
				translation.z--;
			}
			if ( _isKeyPressed( SDL_SCANCODE_A ) || _isKeyPressed( SDL_SCANCODE_LEFT ) )
			{
				translation.x++;
			}
			if ( _isKeyPressed( SDL_SCANCODE_D ) || _isKeyPressed( SDL_SCANCODE_RIGHT ) )
			{
				translation.x--;
			}
			if ( _isKeyPressed( SDL_SCANCODE_R ) )
			{
				translation.y++;
			}
			if ( _isKeyPressed( SDL_SCANCODE_F ) )
			{
				translation.y--;
			}

			if ( translation == VEC3F_ZERO )
			{
				return;
			}

			translation *= VTX_SETTING().translationSpeed;
			translation *= p_deltaTime;

			if ( _isKeyPressed( SDL_SCANCODE_LSHIFT ) )
			{
				translation *= VTX_SETTING().translationFactorSpeed;
			}
			if ( _isKeyPressed( SDL_SCANCODE_LCTRL ) )
			{
				translation /= VTX_SETTING().translationFactorSpeed;
			}

			_camera.move( translation );
		}

		void Freefly::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_SPACE )
			{
				_camera.print();
			}
		}

	} // namespace Controller
} // namespace VTX
