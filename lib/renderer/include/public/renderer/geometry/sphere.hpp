#ifndef __VTX_RENDERER_GEOMETRY_SPHERE__
#define __VTX_RENDERER_GEOMETRY_SPHERE__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	struct Sphere : BaseGeometry
	{
		MapUIDRange ranges;

		void buildDrawRanges()
		{
			drawRanges.firsts.clear();
			drawRanges.counts.clear();

			IndexRangeList allRanges;
			for ( const auto & [ _, range ] : ranges )
			{
				allRanges.addRange( range );
			}

			allRanges.toStdVectorsFirstCount( drawRanges.firsts, drawRanges.counts );
		}

		// Compiled draw ranges.
		DrawRangeArray drawRanges;
	};

} // namespace VTX::Renderer::Geometry

#endif
