#ifndef __VTX_CAMERA__
#define __VTX_CAMERA__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "generic/base_printable.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Object3D
	{
		class Camera : public Generic::BasePrintable
		{
		  public:
			Camera() { _updateRotation(); };

			inline const Vec3f & getPosition() const { return _position; }
			inline const Quatd & getRotation() const { return _rotation; }
			inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
			inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }

			inline void setScreenSize( const uint p_width, const uint p_height )
			{
				_screenWidth  = float( p_width );
				_screenHeight = float( p_height );
				_updateProjectionMatrix();
			}

			inline void setPosition( const Vec3f & p_position )
			{
				_position = p_position;
				_updateViewMatrix();
			}

			inline void setRotation( const Quatf & p_rotation )
			{
				_rotation	 = p_rotation;
				_eulerAngles = Util::Math::quaternionToEuler( _rotation );
				_updateRotation();
			}

			inline void set( const Vec3f & p_position, const Quatf & p_rotation )
			{
				setPosition( p_position );
				setRotation( p_rotation );
			}

			void move( const Vec3f & );
			void moveFront( const float );
			void moveLeft( const float );
			void moveUp( const float );

			void rotate( const Vec3d & );
			void rotatePitch( const float );
			void rotateYaw( const float );
			void rotateRoll( const float );
			void rotateAround( const Vec3f &, const Vec3f &, const float );

			virtual void print() const;

		  private:
			float _screenWidth	= 0.f;
			float _screenHeight = 0.f;
			float _near			= CAMERA_NEAR;
			float _far			= CAMERA_FAR;
			float _fov			= CAMERA_FOV;

			Vec3f _position	   = VEC3F_Z * 50.f;
			Quatd _rotation	   = Quatd();
			Vec3d _eulerAngles = Vec3d(); // [pitch, yaw, roll]

			Vec3f _front = -VEC3F_Z;
			Vec3f _left	 = -VEC3F_X;
			Vec3f _up	 = VEC3F_Y;
			Mat4f _viewMatrix;
			Mat4f _projectionMatrix;

			void _updateRotation();
			void _updateViewMatrix();
			// TODO: call it when changing near, far, fov
			void _updateProjectionMatrix();

		}; // namespace Camera
	}	   // namespace Object3D
} // namespace VTX
#endif
