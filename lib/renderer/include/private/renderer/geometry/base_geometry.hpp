#ifndef __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__
#define __VTX_RENDERER_GEOMETRY_BASE_GEOMETRY__

#include "renderer/descriptors.hpp"
#include <util/math.hpp>

namespace VTX::Renderer::Geometry
{

	struct BaseGeometry
	{
		virtual ~BaseGeometry() = default;

		Util::Math::RangeList<size_t> ranges;
		Blocks						  blocks;
	};

} // namespace VTX::Renderer::Geometry

#endif
