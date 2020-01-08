#ifndef __VTX_COLOR__
#define __VTX_COLOR__

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
			static inline Vec3f randomPastelColor() { return Math::randomVec3f() * 0.5f + 0.5f; }
		} // namespace Color
	}	  // namespace Util
} // namespace VTX

#endif
