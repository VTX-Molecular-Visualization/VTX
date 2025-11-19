#ifndef __VTX_UTIL_MATH_INTERPOLATORS__
#define __VTX_UTIL_MATH_INTERPOLATORS__

#include "util/math.hpp"
#include "util/types.hpp"

namespace VTX::Util::Math::Interpolators
{

	/**
	 * @brief Linear interpolation.
	 */
	struct Lerp
	{
		inline Vec3f operator()( const Vec3f & p_a, const Vec3f & p_b, const float p_t ) const
		{
			return Util::Math::lerp( p_a, p_b, p_t );
		}
		inline Quatf operator()( const Quatf & p_a, const Quatf & p_b, const float p_t ) const
		{
			return Util::Math::lerp( p_a, p_b, p_t );
		}
	};

	/**
	 * @brief Ease in-out interpolation.
	 */
	struct EaseInOut
	{
		inline Vec3f operator()( const Vec3f & p_a, const Vec3f & p_b, const float p_t ) const
		{
			return Util::Math::easeInOutInterpolation( p_a, p_b, p_t );
		}

		inline Quatf operator()( const Quatf & p_a, const Quatf & p_b, const float p_t ) const
		{
			return Util::Math::easeInOutInterpolation( p_a, p_b, p_t );
		}
	};

} // namespace VTX::Util::Math::Interpolators

#endif
