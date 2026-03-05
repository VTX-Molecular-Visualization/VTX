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
			indirectBuffer = "Indirect.Spheres";
		}

		void construct( const SystemData & p_data )
		{
			if ( _ranges.contains( p_data.uid ) )
			{
				return;
			}

			const Index count = p_data.data.getAtomCount();

			assert( count > 0 );
			assert( p_data.atomUids.size() == count );
			assert( p_data.radii.size() == count );

			_addRange( p_data.uid, count );
		}

		void uploadIndexes(
			Context::ContextWrapper & p_context,
			const SystemUID,
			std::span<const uint32_t> p_data
		) override
		{
			throw GraphicException( "This geometry does not support index buffer upload." );
		}
	};

} // namespace VTX::Renderer::Geometry

#endif
