#ifndef __VTX__BSPLINE__
#define __VTX__BSPLINE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "util/math.hpp"
#include <vector>

namespace VTX
{
	namespace Math
	{
		class BSpline
		{
		  public:
			BSpline() { _points = std::vector<Vec3f>(); }
			explicit BSpline( const uint p_size ) { _points = std::vector<Vec3f>( p_size ); }

			inline void						  addPoint( const Vec3f & p_point ) { _points.emplace_back( p_point ); }
			inline void						  setPoint( const int i, const Vec3f & p_point ) { _points[ i ] = p_point; }
			inline const std::vector<Vec3f> & getPoints() const { return _points; }

			inline void		   setPoint( const uint p_pos, const Vec3f & p_point ) { _points[ p_pos ] = p_point; }
			inline const Vec3f getPoint( const uint p_pos ) const { return _points[ p_pos ]; }
			inline void copyPoint( const uint p_src, const uint p_dest ) { _points[ p_dest ] = _points[ p_src ]; }

			inline void shiftPoints()
			{
				for ( uint i = 0; i < _points.size() - 1; ++i )
				{
					setPoint( i, getPoint( i + 1 ) );
				}
			}

		  private:
			std::vector<Vec3f> _points;
		};
	} // namespace Math
} // namespace VTX

#endif
