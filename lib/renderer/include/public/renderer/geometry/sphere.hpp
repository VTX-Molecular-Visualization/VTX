#ifndef __VTX_RENDERER_GEOMETRY_SPHERE__
#define __VTX_RENDERER_GEOMETRY_SPHERE__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	class Sphere : public BaseGeometry
	{
	  public:
		void construct( const SystemData & p_data )
		{
			assert( not _ranges.contains( p_data.uid ) );

			const Index count = p_data.data.getAtomCount();

			assert( count > 0 );
			assert( p_data.atomUids.size() == count );
			assert( p_data.radii.size() == count );

			addRange( p_data.uid, count );
		}
	};

} // namespace VTX::Renderer::Geometry

#endif
