#ifndef __VTX_RENDERER_GEOMETRY_SPHERE__
#define __VTX_RENDERER_GEOMETRY_SPHERE__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	class Sphere : public BaseGeometry
	{
	  public:
		Sphere()
		{
			vertexLayout   = "Atoms";
			indexBuffer	   = "Index.Atoms";
			indirectBuffer = "Indirect.Spheres";
		}

		void construct( const SystemData & p_data )
		{
			if ( _resources.contains( p_data.uid ) )
			{
				return;
			}

			const Index count = p_data.data.getAtomCount();

			assert( count > 0 );
			assert( p_data.atomUids.size() == count );
			assert( p_data.radii.size() == count );

			_addRange( p_data.uid, count, count );
		}
	};

} // namespace VTX::Renderer::Geometry

#endif
