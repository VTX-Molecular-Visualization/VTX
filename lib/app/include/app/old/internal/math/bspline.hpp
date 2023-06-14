#ifndef __VTX_APP_OLD_INTERNAL_MATH_BSPLINE__
#define __VTX_APP_OLD_INTERNAL_MATH_BSPLINE__

#include <util/math.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Internal::Math
{
	class BSpline
	{
	  public:
		BSpline() { _points.resize( 4 ); }

		inline const std::vector<Vec3f> & getPoints() const { return _points; }

		inline void setPoint( const uint p_pos, const Vec3f & p_point )
		{
			_points[ p_pos ] = p_point;
			_needMatUpdate	 = true;
		}
		inline const Vec3f & getPoint( const uint p_pos ) const { return _points[ p_pos ]; }
		inline void			 copyPoint( const uint p_src, const uint p_dest )
		{
			_points[ p_dest ] = _points[ p_src ];
			_needMatUpdate	  = true;
		}

		void  shiftPoints( const uint p_offset = 1 );
		Vec3f computePoint( const float p_t );
		Vec3f computeTangent( const float p_t );

	  private:
		static const Mat4f BSPLINE_MATRIX;

		std::vector<Vec3f> _points		  = std::vector<Vec3f>();
		Mat43f			   _mat			  = Mat43f();
		bool			   _needMatUpdate = true;

		void _updateMatrix3();
	};
} // namespace VTX::App::Old::Internal::Math

#endif
