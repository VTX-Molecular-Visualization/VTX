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

			inline Vec3f & getPosition() { return _position; }
			inline Quatf & getRotation() { return _rotation; }
			inline Mat4f   getViewMatrix() const { return _viewMatrix; }
			inline Mat4f   getProjectionMatrix() const
			{
				return glm::perspective( glm::radians( _fov ), _screenWidth / _screenHeight, _near, _far );
			}

			inline void setScreenSize( const int p_width, const int p_height )
			{
				_screenWidth  = float( p_width );
				_screenHeight = float( p_height );
			}

			inline void set( const Vec3f p_position, const Quatf p_rotation )
			{
				_position = p_position;
				_rotation = p_rotation;
				_update();
			}

			void moveFront( const float );
			void moveLeft( const float );
			void moveUp( const float );
			void rotateLeft( const float );
			void rotateUp( const float );

			virtual void print() const;

		  private:
			float _screenWidth	= 0.f;
			float _screenHeight = 0.f;
			float _near			= CAMERA_NEAR;
			float _far			= CAMERA_FAR;
			float _fov			= CAMERA_FOV;

			Vec3f _position = VEC3F_Z * 50.f;
			Quatf _rotation = Quatf();
			float _pitch	= 0.f;
			float _yaw		= 0.f;
			float _roll		= 0.f;

			Vec3f _front = -VEC3F_Z;
			Vec3f _left	 = -VEC3F_X;
			Vec3f _up	 = VEC3F_Y;
			Mat4f _viewMatrix;

			void _update();

		}; // namespace Camera
	}	   // namespace Object3D
} // namespace VTX
#endif
