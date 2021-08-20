#ifndef __VTX__AABB__
#define __VTX__AABB__

#include "define.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Math
	{
		struct AABB
		{
			AABB()	= default;
			~AABB() = default;

			explicit AABB( const Vec3f & p_point ) : data { p_point, p_point } {}
			AABB( const Vec3f & p_min, const Vec3f & p_max );
			AABB( const Vec3f & p_center, const float p_radius );

			void invalidate()
			{
				data.minMax.min = VEC3F_MAX;
				data.minMax.max = VEC3F_MIN;
			}

			bool isValid() const { return ( ( data.minMax.min.x <= data.minMax.max.x ) && ( data.minMax.min.y <= data.minMax.max.y ) && ( data.minMax.min.z <= data.minMax.max.z ) ); }

			inline const Vec3f & getMin() const { return data.minMax.min; }
			inline const Vec3f & getMax() const { return data.minMax.max; }

			void extend( const Vec3f & p_point );
			void extend( const AABB & p_aabb );
			void extend( const Vec3f & p_center, const float p_radius );

			void translate( const Vec3f & );

			Vec3f diagonal() const { return data.minMax.max - data.minMax.min; }
			float diameter() const { return isValid() ? Util::Math::length( diagonal() ) : 0.f; }
			float radius() const { return isValid() ? diameter() * 0.5f : 0.f; }
			Vec3f centroid() const { return ( data.minMax.min + data.minMax.max ) * 0.5f; }
			uint  largestAxis() const
			{
				const Vec3f d = diagonal();
				if ( d.x > d.y && d.x > d.z )
					return 0;
				else if ( d.y > d.z )
					return 1;
				else
					return 2;
			}
			float area() const
			{
				const Vec3f d = diagonal();
				return isValid() ? ( d.x * d.y + d.y * d.z + d.z * d.x ) * 2.f : 0.f;
			}

			Vec3f offset( const Vec3f & p_pt ) const
			{
				Vec3f o = p_pt - data.minMax.min;
				if ( data.minMax.max.x > data.minMax.min.x )
					o.x /= data.minMax.max.x - data.minMax.min.x;
				if ( data.minMax.max.y > data.minMax.min.y )
					o.y /= data.minMax.max.y - data.minMax.min.y;
				if ( data.minMax.max.z > data.minMax.min.z )
					o.z /= data.minMax.max.z - data.minMax.min.z;
				return o;
			}
			float offset( const Vec3f & p_pt, const uint p_dim ) const
			{
				float o = p_pt[ p_dim ] - data.minMax.min[ p_dim ];
				if ( data.minMax.max[ p_dim ] > data.minMax.min[ p_dim ] )
					o /= data.minMax.max[ p_dim ] - data.minMax.min[ p_dim ];
				return o;
			}

			bool intersect( const Vec3f & p_rayPosition,
							const Vec3f & p_rayInvDir,
							const Vec3i & p_isDirNeg,
							const float	  p_tMin,
							const float	  p_tMax ) const;

			static AABB join( const AABB & p_aabb1, const AABB & p_aabb2 );
			union
			{
				struct 
				{
					Vec3f min;
					Vec3f max;
				} minMax;
				Vec3f limits[ 2 ] = { VEC3F_MAX, VEC3F_MIN };
			} data;
		};
	} // namespace Math
} // namespace VTX

#endif
