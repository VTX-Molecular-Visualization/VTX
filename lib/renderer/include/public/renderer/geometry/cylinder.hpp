#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	struct Cylinder : public BaseGeometry<DrawRangeElements>
	{
	  public:
		void construct( const SystemData & p_data )
		{
			assert( not _ranges.contains( p_data.uid ) );

			addRange( p_data.uid, p_data.data.getBondCount() );
		}
	};
} // namespace VTX::Renderer::Geometry

#endif
