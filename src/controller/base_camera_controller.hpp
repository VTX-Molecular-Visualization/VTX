#ifndef __VTX_BASE_GAMEPAD_CONTROLLER__
#define __VTX_BASE_GAMEPAD_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"
#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "math/aabb.hpp"
#include "object3d/camera.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Controller
	{
		class BaseCameraController : public BaseMouseController, public BaseKeyboardController
		{
		  public:
			explicit BaseCameraController( Object3D::Camera & p_camera ) : _camera( p_camera ) {}
			virtual ~BaseCameraController() = default;

			void update( const float & p_deltaTime ) override
			{
				if ( _isOrienting )
				{
					_orientTime += p_deltaTime;

					if ( _orientTime >= ORIENT_DURATION )
					{
						_updateOrient( 1.f );
						_isOrienting = false;
					}
					else
					{
						_updateOrient( _orientTime / ORIENT_DURATION );
					}
				}
				else
				{
					_updateInputs( p_deltaTime );
				}
			}

			virtual void reset() = 0;

			void orient( const Math::AABB & p_aabb )
			{
				_isOrienting			= true;
				_orientTime				= 0.f;
				_orientStartingPosition = _camera.getPosition();

				const float targetDistance = p_aabb.radius() / ( tan( Util::Math::radians( _camera.getFov() ) * 0.5f ) );
				_orientTargetPosition	   = p_aabb.centroid() - _camera.getFront() * targetDistance;

				_isOrienting = Util::Math::distance( _orientStartingPosition, _orientTargetPosition ) > 1e-4f;
			}

		  protected:
			const float ORIENT_DURATION = 2.f;

			Object3D::Camera & _camera;

			bool  _isOrienting			  = false;
			float _orientTime			  = 0.f;
			Vec3f _orientStartingPosition = VEC3F_ZERO;
			Vec3f _orientTargetPosition	  = VEC3F_ZERO;

			virtual void _updateInputs( const float & ) = 0;
			virtual void _updateOrient( const float & ) = 0;
		};
	} // namespace Controller
} // namespace VTX
#endif
