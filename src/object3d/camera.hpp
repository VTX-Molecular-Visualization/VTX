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
			Camera();

			inline const Vec3f & getPosition() const { return _position; }
			inline const Quatd & getRotation() const { return _rotation; }
			inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
			inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }

			inline const Vec3f & getFront() const { return _front; }
			inline const Vec3f & getLeft() const { return _left; }
			inline const Vec3f & getUp() const { return _up; }
			inline const float	 getNear() const { return _near; }
			inline const float	 getFar() const { return _far; }
			inline const float	 getFov() const { return _fov; }

			inline void setScreenSize( const uint p_width, const uint p_height )
			{
				_screenWidth  = float( p_width );
				_screenHeight = float( p_height );
				_aspectRatio  = _screenWidth / _screenHeight;
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

			inline void setNear( const float p_near )
			{
				// Avoid too little value.
				_near = Util::Math::max( 1e-1f, p_near );
				_updateProjectionMatrix();
			}
			inline void setFar( const float p_far )
			{
				// Avoid too little value.
				_far = Util::Math::max( 1e-1f, p_far );
				_updateProjectionMatrix();
			}
			inline void setFov( const float p_fov )
			{
				_fov = p_fov;
				_updateProjectionMatrix();
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

			void setPerspective( const bool p_perspective )
			{
				_isPerspective = p_perspective;
				_updateProjectionMatrix();
			}

			virtual void print() const;

		  private:
			float _screenWidth	= 1.f;
			float _screenHeight = 1.f;
			float _aspectRatio	= 1.f;
			float _near			= 1e-1f;
			float _far			= 1e4f;
			float _fov			= 60.f;

			Vec3f _position	   = VEC3F_Z * 50.f;
			Quatd _rotation	   = Quatd( 1.0, { 0.0, 0.0, 0.0 } );
			Vec3d _eulerAngles = Vec3d(); // [pitch, yaw, roll]

			Vec3f _front = -VEC3F_Z;
			Vec3f _left	 = -VEC3F_X;
			Vec3f _up	 = VEC3F_Y;
			Mat4f _viewMatrix;
			Mat4f _projectionMatrix;
			bool  _isPerspective = true;

			void _updateRotation();
			void _updateViewMatrix();
			void _updateProjectionMatrix();
		}; // namespace Camera
	}	   // namespace Object3D
} // namespace VTX
#endif
