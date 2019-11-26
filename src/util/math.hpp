#ifndef __VTX__MATH__
#define __VTX__MATH__

#include "../defines.hpp"

namespace VTX
{
	namespace Util
	{
		namespace Math
		{
			inline Vec3f min( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z );
			}
			inline Vec3f max( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z );
			}
		} // namespace Math
	}	  // namespace Util
} // namespace VTX

#endif
