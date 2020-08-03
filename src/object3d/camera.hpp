#ifndef __VTX_CAMERA__
#define __VTX_CAMERA__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Object3D
	{
		class Camera
		{
		  public:
			Camera();

			inline const Vec3d & getPosition() const { return _position; }
			inline const Quatd & getRotation() const { return _rotation; }

			inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
			inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }

			inline const uint  getScreenWidth() const { return _screenWidth; }
			inline const uint  getScreenHeight() const { return _screenHeight; }
			inline const float getAspectRatio() const { return _aspectRatio; }

			inline const Vec3d & getFront() const { return _front; }
			inline const Vec3d & getLeft() const { return _left; }
			inline const Vec3d & getUp() const { return _up; }

			inline const float getNear() const { return _near; }
			inline const float getFar() const { return _far; }
			inline const float getFov() const { return _fov; }

			inline void setScreenSize( const uint p_width, const uint p_height )
			{
				_screenWidth  = p_width;
				_screenHeight = p_height;
				_aspectRatio  = (float)_screenWidth / _screenHeight;
				_updateProjectionMatrix();
			}

			inline void setPosition( const Vec3d & p_position )
			{
				_position = p_position;
				_updateViewMatrix();
			}

			inline void setRotation( const Quatd & p_rotation )
			{
				_rotation = Util::Math::normalize( p_rotation );
				_updateRotation();
			}

			inline void set( const Vec3d & p_position, const Quatd & p_rotation )
			{
				_position = p_position;
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

			inline void setRotationAround( const Quatd & p_rotation, const Vec3d & p_target, const double p_distance )
			{
				_rotation = Util::Math::normalize( p_rotation );
				_position = _rotation * Vec3d( 0.0, 0.0, p_distance ) + p_target;
				_updateRotation();
			}

			void move( const Vec3d & );
			void moveFront( const double );
			void moveLeft( const double );
			void moveUp( const double );

			void rotate( const Vec3d & );
			void rotatePitch( const double );
			void rotateYaw( const double );
			void rotateRoll( const double );

			void rotateAround( const Quatd &, const Vec3d &, const double );

			void setPerspective( const bool p_perspective )
			{
				_isPerspective = p_perspective;
				_updateProjectionMatrix();
			}

			void print() const;

		  private:
			uint  _screenWidth	= 1u;
			uint  _screenHeight = 1u;
			float _aspectRatio	= 1.f;
			float _near			= 1e-1f;
			float _far			= 1e4f;
			float _fov			= 60.f;

			Vec3d _position = VEC3F_Z * 50.f;
			Quatd _rotation = Quatd( 1.0, { 0.0, 0.0, 0.0 } );

			Vec3d _front = -VEC3F_Z;
			Vec3d _left	 = -VEC3F_X;
			Vec3d _up	 = VEC3F_Y;

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
