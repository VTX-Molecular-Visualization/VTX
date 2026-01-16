#ifndef __VTX_RENDERER_GEOMETRY_SPHERE__
#define __VTX_RENDERER_GEOMETRY_SPHERE__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{
	struct Sphere : BaseGeometry
	{
		RangeList	   rangeList;
		DrawRangeArray drawRanges;
	};

} // namespace VTX::Renderer::Geometry

#endif
