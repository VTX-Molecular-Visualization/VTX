#ifndef __VTX_BASE_CAMERA_CONTROLLER__
#define __VTX_BASE_CAMERA_CONTROLLER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_controller.hpp"
#include "base_gamepad_controller.hpp"
#include "base_keyboard_controller.hpp"
#include "base_mouse_controller.hpp"
#include "math/aabb.hpp"
#include "object3d/camera.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		class Camera;
	}

	namespace Controller
	{
		class BaseCameraController :
			public BaseMouseController,
			public BaseKeyboardController,
			public BaseGamepadController
		{
		  public:
			explicit BaseCameraController( Object3D::Camera & p_camera ) : _camera( p_camera ) {}
			virtual ~BaseCameraController() = default;

			inline const bool	 isOrienting() const { return _isOrienting; }
			inline const Vec3f & getOrientStartingPosition() const { return _orientStartingPosition; }
			inline const Vec3f & getOrientTargetPosition() const { return _orientTargetPosition; }

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

			virtual void orient( const Math::AABB & p_aabb )
			{
				_orientTime = 0.f;
				_computeOrientPositions( p_aabb );
			}

		  protected:
			const float ORIENT_DURATION	 = 0.5f;
			const float ORIENT_THRESHOLD = 1e-4f;

			Object3D::Camera & _camera;

			bool  _isOrienting			  = false;
			float _orientTime			  = 0.f;
			Vec3f _orientStartingPosition = VEC3F_ZERO;
			Vec3f _orientTargetPosition	  = VEC3F_ZERO;

			virtual void _updateInputs( const float & )						  = 0;
			virtual void _computeOrientPositions( const Math::AABB & p_aabb ) = 0;
			virtual void _updateOrient( const float & )						  = 0;
		};
	} // namespace Controller
} // namespace VTX
#endif
