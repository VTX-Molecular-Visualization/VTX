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
			inline const Vec3f getPoint( const uint p_pos ) const { return _points[ p_pos ]; }
			inline void		   copyPoint( const uint p_src, const uint p_dest )
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
			const Vec3f getPoint( const float p_t ) const
			{
				// return Util::Math::catmullRomInterpolation(
				//	_points[ 0 ], _points[ 1 ], _points[ 2 ], _points[ 3 ], p_t );

				float s;
				int	  i, j, k;
				Vec4f tmp	= Vec4f();
				Vec3f point = Vec3f();

				for ( i = 0; i < 4; i++ )
				{
					tmp[ i ] = std::pow( p_t, 3 - i );
				}

				for ( j = 0; j < 3; j++ )
				{
					s = 0;
					for ( k = 0; k < 4; k++ )
					{
						s += tmp[ k ] * _mat[ k ][ j ];
					}

					point[ j ] = s;
				}

				return point;
			}

			// Calculates the tangent vector of the spline at t.
			const Vec3f getTangent( const float p_t ) const
			{
				float s;
				int	  i, j, k;
				Vec4f tmp	  = Vec4f();
				Vec3f tangent = Vec3f();

				for ( i = 0; i < 4; i++ )
				{
					if ( i < 3 ) { tmp[ i ] = ( 3 - i ) * std::pow( p_t, 2 - i ); }
					else
					{
						tmp[ i ] = 0.f;
					}
				}

				for ( j = 0; j < 3; j++ )
				{
					s = 0;
					for ( k = 0; k < 4; k++ )
					{
						s += tmp[ k ] * _mat[ k ][ j ];
					}
					tangent[ j ] = s;
				}

				return tangent;
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
