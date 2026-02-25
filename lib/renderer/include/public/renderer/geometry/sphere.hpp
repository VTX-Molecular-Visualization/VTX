#ifndef __VTX_RENDERER_GEOMETRY_SPHERE__
#define __VTX_RENDERER_GEOMETRY_SPHERE__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	struct Sphere : BaseGeometry<DrawRangeArray>
	{
		void construct( const SystemData & p_data )
		{
			assert( not ranges.contains( p_data.uid ) );

			const Index count = static_cast<Index>( p_data.frame.size() );

			assert( count > 0 );
			assert( p_data.atomUids.size() == count );
			assert( p_data.radii.size() == count );

			addRange( p_data.uid, count );
		}
	};

} // namespace VTX::Renderer::Geometry

#endif
