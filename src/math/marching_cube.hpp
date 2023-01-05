#ifndef __VTX__MARCHING_CUBE__
#define __VTX__MARCHING_CUBE__

#include "define.hpp"
#include <vector>

namespace VTX
{
	namespace Math
	{
		class MarchingCube
		{
		  public:
			static const std::vector<std::pair<uint, uint>> EDGE_TO_VERTICES;
			static const uint								EDGE_TABLE[ 256 ];
			static const int								TRIANGLE_TABLE[ 256 ][ 16 ];

			MarchingCube() = default;

			struct GridCell
			{
				Vec3f vertex[ 8 ];
				float value[ 8 ];
			};

			std::vector<std::vector<Vec4f>> triangulateCell( const GridCell & p_gridCell,
															 const float	  p_isovalue ) const;

		  private:
			uint _computeCubeIndex( const GridCell & p_gridCell, const float p_isovalue ) const;

			Vec4f _interpolate( const Vec3f & p_point1,
								const float	  p_val1,
								const Vec3f & p_point2,
								const float	  p_val2,
								const float	  p_isovalue ) const;

			std::vector<Vec4f> _getIntersectionCoordinates( const GridCell & p_gridCell,
															const float		 p_isovalue,
															const uint		 p_cubeIndex ) const;

			std::vector<std::vector<Vec4f>> _getTriangles( const std::vector<Vec4f> & p_intersections,
														   const uint				  p_cubeIndex ) const;
		}; // namespace Math
	}	   // namespace Math
} // namespace VTX
#endif
