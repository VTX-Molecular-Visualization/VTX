#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	struct Cylinder : BaseGeometry
	{
		MapUIDRange ranges;

		void buildDrawRanges()
		{
			drawRanges.offsets.clear();
			drawRanges.counts.clear();

			IndexRangeList allRanges;
			for ( const auto & [ _, range ] : ranges )
			{
				allRanges.addRange( range );
			}

			allRanges.toStdVectorsFirstCount( drawRanges.offsets, drawRanges.counts );
		}

		// Compiled draw ranges.
		DrawRangeElements drawRanges;
	};
} // namespace VTX::Renderer::Geometry

#endif
