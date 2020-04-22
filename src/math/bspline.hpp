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
			BSpline() { _points.resize( 4 ); }

			inline const std::vector<Vec3f> & getPoints() const { return _points; }

			inline void setPoint( const uint p_pos, const Vec3f & p_point )
			{
				_points[ p_pos ] = p_point;
				_updateMatrix3();
			}
			inline const Vec3f & getPoint( const uint p_pos ) const { return _points[ p_pos ]; }
			inline void			 copyPoint( const uint p_src, const uint p_dest )
			{
				_points[ p_dest ] = _points[ p_src ];
				_updateMatrix3();
			}

			void shiftPoints( const uint p_offset = 1 )
			{
				for ( uint i = 0; i < _points.size() - p_offset; ++i )
				{
					_points[ i ] = _points[ i + p_offset ];
				}
				_updateMatrix3();
			}

			// Calculates the point on the cubic spline corresponding to the parameter value t in [0, 1].
			Vec3f computePoint( const float p_t ) const
			{
				Vec4f tmp = Vec4f();

				for ( int i = 0; i < 4; i++ )
				{
					tmp[ i ] = std::pow( p_t, 3 - i );
				}

				return _mat * tmp;
			}

			// Calculates the tangent vector of the spline at t.
			Vec3f computeTangent( const float p_t ) const
			{
				Vec4f tmp = Vec4f();

				for ( int i = 0; i < 3; i++ )
				{
					tmp[ i ] = ( 3 - i ) * std::pow( p_t, 2 - i );
				}

				return _mat * tmp;
			}

			void _updateMatrix3()
			{
				float s;
				int	  i, j, k;
				for ( i = 0; i < 4; i++ )
				{
					for ( j = 0; j < 3; j++ )
					{
						s = 0;
						for ( k = 0; k < 4; k++ )
						{
							s += BSPLINE_MATRIX[ i ][ k ] * _points[ k ][ j ];
						}
						_mat[ i ][ j ] = s;
					}
				}
			}

		  private:
			std::vector<Vec3f> _points = std::vector<Vec3f>();
			Mat43f			   _mat	   = Mat43f();

			static const Mat4f BSPLINE_MATRIX;
		};
	} // namespace Math
} // namespace VTX

#endif
