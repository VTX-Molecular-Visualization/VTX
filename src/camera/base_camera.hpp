#ifndef __BASE_CAMERA__
#define __BASE_CAMERA__

#include "../defines.hpp"

namespace VTX
{
	namespace Camera
	{
		class BaseCamera
		{
		  public:
			BaseCamera()		  = default;
			virtual ~BaseCamera() = default;

			Mat4f getViewMatrix() const;
			Mat4f getProjectionMatrix() const;
			void  setScreenSize( const int, const int );

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

			Vec3f _pos	 = VEC3F_ZERO;
			Vec3f _front = VEC3F_Z;
			Vec3f _up	 = VEC3F_Y;
			Vec3f _left	 = VEC3F_X;

			float _theta = 0.f;
			float _phi	 = 0.f;
		};
	} // namespace Camera
} // namespace VTX
#endif
