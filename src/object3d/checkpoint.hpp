#ifndef __VTX_CHECKPOINT__
#define __VTX_CHECKPOINT__

#ifdef _MSC_VER
#pragma once
#endif

#include "../defines.hpp"

namespace VTX
{
	namespace Object3D
	{
		class Checkpoint
		{
		  public:
			float _time = 0.f;

			Vec3f _position = VEC3F_ZERO;
			float _theta	= 0.f;
			float _phi		= 4.22369f;

		}; // namespace Camera
	}	   // namespace Object3D
} // namespace VTX
#endif
