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
			virtual void rotateAroundLeft( const float, const Vec3f & ) final;
			virtual void rotateAroundUp( const float, const Vec3f & ) final;

			virtual void printInfo() const;

		  protected:
			float _screenWidth;
			float _screenHeight;
			float _near	 = CAMERA_NEAR;
			float _far	 = CAMERA_FAR;
			float _fov	 = CAMERA_FOV;
			float _theta = 0.f;
			float _phi	 = 4.22369f;

			Vec3f _position = VEC3F_ZERO;
			Vec3f _front	= VEC3F_Z;
			Vec3f _up		= VEC3F_Y;
			Vec3f _left		= VEC3F_X;

			void _update();
		}; // namespace Camera
	}	   // namespace Object3D
} // namespace VTX
#endif
