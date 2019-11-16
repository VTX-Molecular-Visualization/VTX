#ifndef __VTX__AABB__
#define __VTX__AABB__

#include "../defines.hpp"

namespace VTX
{
	namespace Math
	{
		class AABB
		{
		  public:
			AABB()	= default;
			~AABB() = default;

			explicit AABB( const Vec3f & point ) : _min( point ), _max( point ) {}
			AABB( const Vec3f & p_center, const float p_radius );

			inline const Vec3f & min() const { return _min; }
			inline const Vec3f & max() const { return _max; }

			void extend( const Vec3f & p_point );
			void extend( const Vec3f & p_center, const float p_radius );

			Vec3f center() const { return ( _min + _max ) * 0.5f; }
			float radius() const { return glm::length( _max - _min ) * 0.5f; }

		  private:
			Vec3f _min = Vec3f( FLT_MAX );
			Vec3f _max = Vec3f( -FLT_MAX );
		};
	} // namespace Math
} // namespace VTX

#endif
