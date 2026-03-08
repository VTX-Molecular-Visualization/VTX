#ifndef __VTX_RENDERER_GEOMETRY_CYLINDER__
#define __VTX_RENDERER_GEOMETRY_CYLINDER__

#include "base_geometry.hpp"

namespace VTX::Renderer::Geometry
{

	class Cylinder : public BaseGeometry
	{
	  public:
		Cylinder()
		{
			vertexLayout   = "Atoms";
			indexBuffer	   = "Index.Bonds";
			indirectBuffer = "Indirect.Cylinders";
		}

		void construct( const SystemData & p_data )
		{
			if ( _resources.contains( p_data.uid ) )
			{
				return;
			}

			_addRange( p_data.uid, p_data.data.getBondCount() * 2, p_data.data.getAtomCount() );
		}
	};
} // namespace VTX::Renderer::Geometry

#endif
