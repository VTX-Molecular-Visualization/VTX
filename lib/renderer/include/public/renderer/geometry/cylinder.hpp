#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{
	struct Cylinder : BaseGeometry
	{
		RangeList		  rangeList;
		DrawRangeElements drawRanges;
	};
} // namespace VTX::Renderer::Geometry

#endif
