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

			inline const Vec3f & getPosition() const { return _position; }
			inline const Quatf & getRotation() const { return _rotation; }

			inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
			inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }

			inline const uint  getScreenWidth() const { return _screenWidth; }
			inline const uint  getScreenHeight() const { return _screenHeight; }
			inline const float getAspectRatio() const { return _aspectRatio; }

			inline const Vec3f & getFront() const { return _front; }
			inline const Vec3f & getLeft() const { return _left; }
			inline const Vec3f & getUp() const { return _up; }

			inline const float getNear() const { return _near; }
			inline const float getFar() const { return _far; }
			inline const float getFov() const { return _fov; }

			inline void setScreenSize( const uint p_width, const uint p_height )
			{
				_screenWidth  = p_width;
				_screenHeight = p_height;
				_aspectRatio  = (float)_screenWidth / (float)_screenHeight;
				_updateProjectionMatrix();
			}

			inline void setPosition( const Vec3f & p_position )
			{
				_position = p_position;
				_updateViewMatrix();
			}

			inline void setRotation( const Quatf & p_rotation )
			{
				_rotation = Util::Math::normalize( p_rotation );
				_updateRotation();
			}

			inline void setRotation( const Vec3f & p_rotation )
			{
				_rotation = Util::Math::normalize( Util::Math::eulerToQuaternion( p_rotation ) );
				_updateRotation();
			}

			inline void set( const Vec3f & p_position, const Quatf & p_rotation )
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

			inline void setRotationAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance )
			{
				_rotation = Util::Math::normalize( p_rotation );
				_position = _rotation * Vec3f( 0.f, 0.f, p_distance ) + p_target;
				_updateRotation();
			}

			void move( const Vec3f & );
			void moveFront( const float );
			void moveLeft( const float );
			void moveUp( const float );

			void rotate( const Vec3f & );
			void rotatePitch( const float );
			void rotateYaw( const float );
			void rotateRoll( const float );

			void rotateAround( const Quatf &, const Vec3f &, const float );

			void lookAt( const Vec3f &, const Vec3f & );

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

			Vec3f _position; //= VEC3F_Z * 50.f;
			Quatf _rotation = Quatf( 1.f, { 0.f, 0.f, 0.f } );

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
