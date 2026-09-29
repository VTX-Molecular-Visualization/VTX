#ifndef __VTX_RENDERER_GEOMETRY_SPHERE__
#define __VTX_RENDERER_GEOMETRY_SPHERE__

#include "base_geometry.hpp"
#include "renderer/caches.hpp"
#include <string_view>

namespace VTX::Renderer::Geometry
{

	class Sphere : public BaseGeometry
	{
	  public:
		static constexpr std::string_view VERTEX_LAYOUT_ATOMS	   = "Atoms";
		static constexpr std::string_view GEOMETRY_SPHERES		   = "Spheres";
		static constexpr std::string_view INDEX_ATOMS			   = "Index.Atoms";
		static constexpr std::string_view INDIRECT_SPHERES		   = "Indirect.Spheres";
		static constexpr Desc::Binding	  BINDING_INDIRECT_SPHERES = 10;

		Sphere()
		{
			vertexLayout   = Desc::Key { VERTEX_LAYOUT_ATOMS };
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
			if ( not _hasRange( p_handle ) )
			{
				return;
			}

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
