#ifndef __VTX_UTIL_COLOR__
#define __VTX_UTIL_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "math.hpp"

namespace VTX
{
	namespace Util
	{
		namespace Color
		{
			extern const Vec3f black;
			extern const Vec3f white;
			extern const Vec3f red;
			extern const Vec3f green;
			extern const Vec3f blue;
			extern const Vec3f yellow;
			extern const Vec3f magenta;
			extern const Vec3f cyan;

			extern const std::vector<Vec3f> predefinedChainColors;

			static inline Vec3f random() { return Math::randomVec3f(); }
			static inline Vec3f randomPastel() { return Math::randomVec3f() * 0.5f + 0.5f; }
		}; // namespace Color
	}	   // namespace Util
} // namespace VTX

#endif
