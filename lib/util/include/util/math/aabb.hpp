#ifndef __VTX_UTIL_MATH_AABB__
#define __VTX_UTIL_MATH_AABB__

#include "util/constants.hpp"
#include "util/math.hpp"
#include "util/types.hpp"

namespace VTX::Util::Math
{
	/**
	 * @brief Axis Aligned Bounding Box.
	 */
	class AABB
	{
	  public:
		/**
		 * @brief Constructors.
		 */
		AABB() = default;
		AABB( const Vec3f & p_point );
		AABB( const Vec3f & p_min, const Vec3f & p_max );
		AABB( const Vec3f & p_center, const float p_radius );

		/**
		 * @brief Reset min and max points to invalid values.
		 */
		void invalidate();

		/**
		 * @brief Check if the AABB is valid.
		 */
		inline bool isValid() const { return ( ( _min.x <= _max.x ) && ( _min.y <= _max.y ) && ( _min.z <= _max.z ) ); }

		/**
		 * @brief Getters.
		 */
		inline const Vec3f & getMin() const { return _min; }
		inline const Vec3f & getMax() const { return _max; }

		/**
		 * @brief Get all 8 summits of the AABB.
		 */
		std::vector<Vec3f> getSummits() const;

		/**
		 * @brief Extend to include the given point.
		 */
		inline void extend( const Vec3f & p_point )
		{
			_min = Util::Math::min( p_point, _min );
			_max = Util::Math::max( p_point, _max );
		}

		/**
		 * @brief Extend to include the given AABB.
		 */
		inline void extend( const AABB & p_aabb )
		{
			_min = Util::Math::min( _min, p_aabb._min );
			_max = Util::Math::max( _max, p_aabb._max );
		}

		/**
		 * @brief Extend to include the given sphere.
		 */
		inline void extend( const Vec3f & p_center, const float p_radius )
		{
			extend( p_center - p_radius );
			extend( p_center + p_radius );
		}

		/**
		 * @brief Translate the AABB by the given vector.
		 */
		inline void translate( const Vec3f & p_translation )
		{
			_min += p_translation;
			_max += p_translation;
		}

		/**
		 * @brief Compute properties of the AABB.
		 */
		inline Vec3f diagonal() const { return _max - _min; }
		inline float diameter() const { return isValid() ? Util::Math::length( diagonal() ) : 0.f; }
		inline float radius() const { return isValid() ? diameter() * 0.5f : 0.f; }
		inline Vec3f centroid() const { return ( _min + _max ) * 0.5f; }
		uint		 largestAxis() const;
		float		 area() const;
		Vec3f		 offset( const Vec3f & p_pt ) const;
		float		 offset( const Vec3f & p_pt, const uint p_dim ) const;

		/*
		bool intersect(
			const Vec3f & p_rayPosition,
			const Vec3f & p_rayInvDir,
			const Vec3i & p_isDirNeg,
			const float	  p_tMin,
			const float	  p_tMax
		) const;
		*/

		/**
		 * @brief Static join.
		 */
		static AABB join( const AABB & p_aabb1, const AABB & p_aabb2 );

	  private:
		/**
		 * @brief Minimum and maximum points of the AABB.
		 */
		Vec3f _min = VEC3F_MAX;
		Vec3f _max = VEC3F_LOWEST;
	};
} // namespace VTX::Util::Math

#endif
