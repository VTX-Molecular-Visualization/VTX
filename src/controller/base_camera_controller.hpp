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

			void update( const double & p_deltaTime ) override
			{
				if ( _isOrienting )
				{
					_orientTime += p_deltaTime;

					if ( _orientTime >= ORIENT_DURATION )
					{
						_updateOrient( 1.0 );
						_isOrienting = false;
					}
					else
					{
						_updateOrient( 0.0 );
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
				_isOrienting	   = true;
				_orientStartCamera = _camera;
				_orientTime		   = 0.0;
				_orientTargetAabb  = p_aabb;
			}

		  protected:
			const double ORIENT_DURATION = 3.0;

			Object3D::Camera & _camera;

			bool			 _isOrienting = false;
			double			 _orientTime  = 0.0;
			Object3D::Camera _orientStartCamera;
			Math::AABB		 _orientTargetAabb;

			virtual void _updateInputs( const double & ) = 0;
			virtual void _updateOrient( const double & ) = 0;
		};
	} // namespace Controller
} // namespace VTX
#endif
