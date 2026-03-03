#ifndef __VTX_RENDERER_GEOMETRY_GRID__
#define __VTX_RENDERER_GEOMETRY_GRID__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{
	class Grid : public BaseGeometry
	{
	  public:
		Grid()
		{
			vertexLayout   = "Voxels";
			indirectBuffer = "Indirect.Grid";
		}
	};
} // namespace VTX::Renderer::Geometry

#endif
