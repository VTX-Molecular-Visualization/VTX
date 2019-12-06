#ifndef __VTX_CAMERA_ORBIT__
#define __VTX_CAMERA_ORBIT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_camera.hpp"

namespace VTX
{
	namespace Object3D
	{
		namespace Camera
		{
			class CameraOrbit : BaseCamera
			{
			  private:
				Vec3f _target = VEC3F_ZERO;
			};
		} // namespace Camera
	}	  // namespace Object3D
} // namespace VTX
#endif
