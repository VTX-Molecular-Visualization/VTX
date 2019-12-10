#ifndef __VTX_CAMERA__
#define __VTX_CAMERA__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>

namespace VTX
{
	namespace Object3D
	{
		class Camera
		{
		  public:
			Camera() { _update(); };
			virtual ~Camera() = default;

			inline Mat4f getViewMatrix() const { return glm::lookAt( _position, _position + _front, _up ); }
			inline Mat4f getProjectionMatrix() const
			{
				return glm::perspective( glm::radians( _fov ), _screenWidth / _screenHeight, _near, _far );
			}
			inline void setScreenSize( const int p_width, const int p_height )
			{
				_screenWidth  = float( p_width );
				_screenHeight = float( p_height );
			}

			virtual void zoom( const float ) final;
			virtual void moveFront( const float ) final;
			virtual void moveLeft( const float ) final;
			virtual void moveUp( const float ) final;
			virtual void rotateLeft( const float ) final;
			virtual void rotateUp( const float ) final;

			virtual void printInfo() const;

		  protected:
			float _screenWidth;
			float _screenHeight;
			float _near = CAMERA_NEAR;
			float _far	= CAMERA_FAR;
			float _fov	= CAMERA_FOV;

			// Vec3f _position = VEC3F_ZERO;
			// Vec3f _front	= VEC3F_Z;
			// Vec3f _up		= VEC3F_Y;
			// Vec3f _left		= VEC3F_X;

			/*Position : [ 806.803, 1121.98, 712.788 ] Front
		: [ 0.387621, -0.64011, -0.663332 ] Left
		: [ -0.863395, 0, -0.504528 ] Up
		: [ 0.322953, 0.768283, -0.552668 ] Phi : 2.61276 Theta
		: -0.694641 Fov : 60;*/
			Vec3f _position
				//= VEC3F_ZERO;
				//= Vec3f(-1.9233e-07, 0, -2.2);
				//= Vec3f(28.6974, 107.456, -72.8066);
				//= Vec3f(25.3197f, 928.787f, -1023.249f);
				= Vec3f( 1444.52, 582.281, 990.344 );
			//= Vec3f(377.618, 1228.58, 0.0116177);
			//= Vec3f(697.252, 199.743, 1706.77); // pdv video
			// Vec3f(773.704, 1184.4, 678.455);
			// Vec3f(827.962, 1173.53, 784.935); // 0.f,
			// 0.f, 5.f); // Vec3f(806.803, 1121.98, 712.788);
			Vec3f _front // = VEC3F_Z;
				= Vec3f( -0.888385, -0.0226873, -0.458538 );
			Vec3f _left = VEC3F_X;
			Vec3f _up	= VEC3F_Y;

			float _theta = 0.f;
			float _phi	 = 4.22369f;

			void _update();
		}; // namespace Camera
	}	   // namespace Object3D
} // namespace VTX
#endif
