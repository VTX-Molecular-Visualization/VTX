#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	struct Cylinder : public BaseGeometry
	{
	  public:
		void construct( const SystemData & p_data )
		{
			// assert( not _ranges.contains( p_data.uid ) );

			addRange( p_data.uid, p_data.data.getBondCount() * 2 );
		}
	};
} // namespace VTX::Renderer::Geometry

#endif
