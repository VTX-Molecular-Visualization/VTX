#ifndef __CAMERA_ORBIT__
#define __CAMERA_ORBIT__

#include "base_camera.hpp"

namespace VTX
{
	namespace Camera
	{
		class CameraOrbit : BaseCamera
		{
		  private:
			Vec3f _target = VEC3F_ZERO;
		};

	} // namespace Camera
} // namespace VTX
#endif
