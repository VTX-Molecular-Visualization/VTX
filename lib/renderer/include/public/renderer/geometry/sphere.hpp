#ifndef __VTX_RENDERER_GEOMETRY_SPHERE__
#define __VTX_RENDERER_GEOMETRY_SPHERE__

#include "base_geometry.hpp"
#include "renderer/caches.hpp"

namespace VTX::Renderer::Geometry
{

	class Sphere : public BaseGeometry
	{
	  public:
		inline static const Desc::Key		  VERTEX_LAYOUT_ATOMS	   = "Atoms";
		inline static const Desc::Key		  GEOMETRY_SPHERES		   = "Spheres";
		inline static const Desc::Key		  INDEX_ATOMS			   = "Index.Atoms";
		inline static const Desc::Key		  INDIRECT_SPHERES		   = "Indirect.Spheres";
		inline static constexpr Desc::Binding BINDING_INDIRECT_SPHERES = 20;

		Sphere()
		{
			vertexLayout   = VERTEX_LAYOUT_ATOMS;
			indiceBuffer   = INDEX_ATOMS;
			indirectBuffer = INDIRECT_SPHERES;
		}

		void registerSystem( const Desc::Handle p_handle, const Cache::System & p_data )
		{
			const Index count = p_data.data.topology->getAtomCount();

			assert( count > 0 );
			assert( p_data.data.atomUids->getCount() == count );

			_addRange( p_handle, count, count );

			auto & indiceBuffer = _indices( p_handle );
			indiceBuffer.resize( count );
			std::iota( indiceBuffer.begin(), indiceBuffer.end(), 0 );
		}

		void setVisibility( const Desc::Handle p_handle, const Util::Math::BitSet & p_visibility )
		{
			auto & indiceBuffer = _indices( p_handle );

			indiceBuffer.clear();
			for ( auto i : p_visibility )
			{
				indiceBuffer.emplace_back( static_cast<Index>( i ) );
			}
		}
	};

} // namespace VTX::Renderer::Geometry

#endif
