#include "bspline.hpp"

namespace VTX
{
	namespace Math
	{
		const Mat4f BSpline::BSPLINE_MATRIX = { { -1.f / 6.f, 1.f / 2.0f, -1.f / 2.f, 1.f / 6.f },
												{ 1.f / 2.f, -1.f, 1.f / 2.f, 0.f },
												{ -1.f / 2.f, 0.f, 1.f / 2.f, 0.f },
												{ 1.f / 6.f, 2.f / 3.f, 1.f / 6.f, 0.f } };

		void BSpline::shiftPoints( const uint p_offset )
		{
			for ( uint i = 0; i < _points.size() - p_offset; ++i )
			{
				_points[ i ] = _points[ i + p_offset ];
			}
			_needMatUpdate = true;
		}

		void BSpline::_updateMatrix3()
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
			_needMatUpdate = false;
		}

		// Calculates the point on the cubic spline corresponding to the parameter value t in [0, 1].
		Vec3f BSpline::computePoint( const float p_t )
		{
			if ( _needMatUpdate ) { _updateMatrix3(); }

			Vec4f tmp = Vec4f();

			for ( int i = 0; i < 4; i++ )
			{
				tmp[ i ] = std::pow( p_t, 3 - i );
			}

			return _mat * tmp;
		}

		// Calculates the tangent vector of the spline at t in [0, 1]..
		Vec3f BSpline::computeTangent( const float p_t )
		{
			if ( _needMatUpdate ) { _updateMatrix3(); }

			Vec4f tmp = Vec4f();

			for ( int i = 0; i < 3; i++ )
			{
				tmp[ i ] = ( 3 - i ) * std::pow( p_t, 2 - i );
			}

			return _mat * tmp;
		}

	} // namespace Math
} // namespace VTX
