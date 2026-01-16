#ifndef __VTX_RENDERER_GEOMETRY_RIBBON__
#define __VTX_RENDERER_GEOMETRY_RIBBON__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{
	struct Ribbon : BaseGeometry
	{
		RangeList	   rangeList;
		DrawRangeArray drawRanges;
	};

} // namespace VTX::Renderer::Geometry

#endif
