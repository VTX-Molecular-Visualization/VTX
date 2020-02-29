#include "orbit.hpp"
#include "setting.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Controller
	{
		void Orbit::update( const double p_deltaTime )
		{
			if ( _distance != 0.f )
			{
				if ( _isKeyPressed( SDL_SCANCODE_LSHIFT ) )
				{ _distance *= Setting::Controller::translationFactorSpeed; }
				if ( _isKeyPressed( SDL_SCANCODE_LCTRL ) ) { _distance /= Setting::Controller::translationFactorSpeed; }
				_camera.moveFront( _distance );
				_distance = 0.f;
			}

			if ( _velocityX != 0.f ) { _camera.rotateAround( _target, VEC3F_Y, _velocityX ); }

			_velocityX = Util::Math::lerp( _velocityX, 0.f, (float)p_deltaTime * CONTROLLER_ORBIT_ELASTICITY );
			_velocityY = Util::Math::lerp( _velocityY, 0.f, (float)p_deltaTime * CONTROLLER_ORBIT_ELASTICITY );
		}

		void Orbit::_handleKeyPressedEvent( const SDL_Scancode & p_key )
		{
			if ( p_key == SDL_SCANCODE_SPACE ) { _camera.print(); }
		}

		void Orbit::_handleMouseMotionEvent( const SDL_MouseMotionEvent & p_event )
		{
			if ( _mouseLeftPressed )
			{
				_velocityX += Setting::Controller::rotationSpeed * p_event.yrel
							  * ( Setting::Controller::yAxisInverted ? -1.f : 1.f );
				_velocityY += -Setting::Controller::rotationSpeed * p_event.xrel;
			}
		}

		void Orbit::_handleMouseWheelEvent( const SDL_MouseWheelEvent & p_event )
		{
			_distance += Setting::Controller::translationSpeed * p_event.y * 0.01f;
		}
	} // namespace Controller
} // namespace VTX
