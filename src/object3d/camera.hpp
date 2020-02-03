#ifndef __VTX_CAMERA__
#define __VTX_CAMERA__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_printable.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>

namespace VTX
{
	namespace Object3D
	{
		class Camera : public Generic::BasePrintable
		{
		  public:
			Camera() { _update(); };

			inline Mat4f getViewMatrix() const { return _viewMatrix; }
			inline Mat4f getProjectionMatrix() const
			{
				return glm::perspective( glm::radians( _fov ), _screenWidth / _screenHeight, _near, _far );
			}

			inline void setScreenSize( const int p_width, const int p_height )
			{
				_screenWidth  = float( p_width );
				_screenHeight = float( p_height );
			}

			void moveFront( const float );
			void moveLeft( const float );
			void moveUp( const float );
			void rotateLeft( const float );
			void rotateUp( const float );

			virtual void print() const override
			{
				VTX_INFO( "Eye: " + glm::to_string( _eye ) );
				VTX_INFO( "Front: " + glm::to_string( _front ) );
				VTX_INFO( "Left: " + glm::to_string( _left ) );
				VTX_INFO( "Up: " + glm::to_string( _up ) );
				VTX_INFO( "Pitch: " + std::to_string( _pitch ) );
				VTX_INFO( "Yaw: " + std::to_string( _yaw ) );
				VTX_INFO( "Roll: " + std::to_string( _roll ) );
				VTX_INFO( "Quat: " + glm::to_string( _quat ) );
			}

		  private:
			float _screenWidth	= 0.f;
			float _screenHeight = 0.f;
			float _near			= CAMERA_NEAR;
			float _far			= CAMERA_FAR;
			float _fov			= CAMERA_FOV;

			Quatf _quat;
			Vec3f _eye = VEC3F_ZERO;
			Vec3f _front;
			Vec3f _left;
			Vec3f _up;
			Mat4f _viewMatrix;

			float _pitch = 0.f;
			float _yaw	 = 0.f;
			float _roll	 = 0.f;

			void _update();

		}; // namespace Camera
	}	   // namespace Object3D
} // namespace VTX
#endif
