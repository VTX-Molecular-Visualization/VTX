#ifndef __VTX__COLOR__
#define __VTX__COLOR__

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
			inline Vec3f randomPastelColor() { return Math::randomVec3f() * 0.5f + 0.5f; }
		} // namespace Color
	}	  // namespace Util
} // namespace VTX

#endif
