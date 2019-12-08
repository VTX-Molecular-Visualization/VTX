#ifndef __VTX_BASE_CAMERA__
#define __VTX_BASE_CAMERA__

#ifdef _MSC_VER
#pragma once
#endif

#include "../../defines.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace VTX
{
	namespace Object3D
	{
		namespace Camera
		{
			class BaseCamera
			{
			  public:
				BaseCamera()		  = default;
				virtual ~BaseCamera() = default;

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

			  protected:
				virtual void _zoom( const float ) final;
				virtual void _moveFront( const float ) final;
				virtual void _moveLeft( const float ) final;
				virtual void _moveUp( const float ) final;
				virtual void _rotateLeft( const float ) final;
				virtual void _rotateUp( const float ) final;

				void _update();

			  private:
				float _screenWidth;
				float _screenHeight;
				float _near = CAMERA_NEAR;
				float _far	= CAMERA_FAR;
				float _fov	= CAMERA_FOV;

				Vec3f _position = VEC3F_ZERO;
				Vec3f _front	= VEC3F_Z;
				Vec3f _up		= VEC3F_Y;
				Vec3f _left		= VEC3F_X;

				float _theta = 0.f;
				float _phi	 = 0.f;
			};
		} // namespace Camera
	}	  // namespace Object3D
} // namespace VTX
#endif
