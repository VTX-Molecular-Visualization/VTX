#ifndef __VTX__MATH__
#define __VTX__MATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "../define.hpp"
#include <random>

#undef min
#undef max

namespace VTX
{
	namespace Util
	{
		namespace Math
		{
			static std::random_device					 rd;
			static std::mt19937							 gen( 79 );
			static std::uniform_real_distribution<float> dis( 0.f, 1.f );

			static inline Vec3f min( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z );
			}

			static inline Vec3f max( const Vec3f & a, const Vec3f & b )
			{
				return Vec3f( a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z );
			}

			static inline Vec3f randomVec3f() { return Vec3f( dis( gen ), dis( gen ), dis( gen ) ); }
		} // namespace Math
	}	  // namespace Util
} // namespace VTX

#endif
