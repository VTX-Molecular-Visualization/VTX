#ifndef __VTX__MATH__
#define __VTX__MATH__

#include "../defines.hpp"

namespace VTX
{
	namespace Util
	{
		class Math
		{
		  public:
			static inline Vec3f min( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z );
			}
			static inline Vec3f max( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z );
			}

		  private:
			Math() = default;
		};
	} // namespace Util
} // namespace VTX

#endif
