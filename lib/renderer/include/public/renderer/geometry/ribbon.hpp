#ifndef __VTX_RENDERER_GEOMETRY_RIBBON__
#define __VTX_RENDERER_GEOMETRY_RIBBON__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	struct Ribbon : BaseGeometry
	{
		MapUIDRange ranges;

		// Compiled draw ranges.
		DrawRangeArray drawRanges;
	};

} // namespace VTX::Renderer::Geometry

#endif
