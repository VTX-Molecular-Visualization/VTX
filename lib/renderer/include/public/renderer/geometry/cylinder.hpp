#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	struct Cylinder : BaseGeometry<DrawRangeElements>
	{
		void construct( const SystemData & p_data )
		{
			assert( not ranges.contains( p_data.uid ) );

			const Index count = static_cast<Index>( p_data.data.bondPairAtomIndexes.size() );
			addRange( p_data.uid, count );
		}
	};
} // namespace VTX::Renderer::Geometry

#endif
