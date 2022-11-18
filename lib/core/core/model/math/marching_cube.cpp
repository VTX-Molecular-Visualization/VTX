#include "marching_cube.hpp"

namespace VTX
{
	namespace Math
	{

		std::vector<std::vector<Vec3f>> MarchingCube::triangulateCell( const GridCell & p_gridCell,
																	   const float		p_isovalue ) const
		{
			int									  cubeIndex		= _computeCubeIndex( p_gridCell, p_isovalue );
			const std::vector<Vec3f>			  intersections = _getIntersectionCoordinates( p_gridCell, p_isovalue );
			const std::vector<std::vector<Vec3f>> triangles		= _getTriangles( intersections, cubeIndex );

			return triangles;
		}

		uint MarchingCube::_computeCubeIndex( const GridCell & p_gridCell, const float p_isovalue ) const
		{
			uint cubeIndex = 0;
			for ( uint i = 0; i < 8; ++i )
			{
				if ( p_gridCell.value[ i ] < p_isovalue )
				{
					cubeIndex |= ( 1 << i );
				}
			}
			return cubeIndex;
		}

		Vec3f MarchingCube::_interpolate( const Vec3f & p_point1,
										  const float	p_val1,
										  const Vec3f & p_point2,
										  const float	p_val2,
										  const float	p_isovalue ) const
		{
			Vec3f		interpolated;
			const float mu = ( p_isovalue - p_val1 ) / ( p_val2 - p_val1 );

			interpolated.x = mu * ( p_point2.x - p_point1.x ) + p_point1.x;
			interpolated.y = mu * ( p_point2.y - p_point1.y ) + p_point1.y;
			interpolated.z = mu * ( p_point2.z - p_point1.z ) + p_point1.z;

			return interpolated;
		}

		std::vector<Vec3f> MarchingCube::_getIntersectionCoordinates( const GridCell & p_gridCell,
																	  const float	   p_isovalue ) const
		{
			std::vector<Vec3f> intersections( 12 );

			const uint cubeIndex		= _computeCubeIndex( p_gridCell, p_isovalue );
			uint	   intersectionsKey = _EDGE_TABLE[ cubeIndex ];

			uint idx = 0;
			while ( intersectionsKey )
			{
				if ( intersectionsKey & 1 )
				{
					const uint	v1				  = _EDGE_TO_VERTICES[ idx ].first;
					const uint	v2				  = _EDGE_TO_VERTICES[ idx ].second;
					const Vec3f intersectionPoint = _interpolate( p_gridCell.vertex[ v1 ],
																  p_gridCell.value[ v1 ],
																  p_gridCell.vertex[ v2 ],
																  p_gridCell.value[ v2 ],
																  p_isovalue );
					intersections[ idx ]		  = intersectionPoint;
				}
				idx++;
				intersectionsKey >>= 1;
			}

			return intersections;
		}

		std::vector<std::vector<Vec3f>> MarchingCube::_getTriangles( const std::vector<Vec3f> & p_intersections,
																	 const uint					p_cubeIndex ) const
		{
			std::vector<std::vector<Vec3f>> triangles = std::vector<std::vector<Vec3f>>();
			for ( uint i = 0; _TRIANGLE_TABLE[ p_cubeIndex ][ i ] != -1; i += 3 )
			{
				std::vector<Vec3f> triangle( 3 );
				for ( uint j = 0; j < 3; ++j )
				{
					triangle[ j ] = p_intersections[ _TRIANGLE_TABLE[ p_cubeIndex ][ i + j ] ];
				}
				triangles.push_back( triangle );
			}

			return triangles;
		}

	} // namespace Math
} // namespace VTX
