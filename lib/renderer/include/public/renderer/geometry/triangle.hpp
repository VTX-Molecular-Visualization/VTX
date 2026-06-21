#ifndef __VTX_RENDERER_GEOMETRY_TRIANGLE__
#define __VTX_RENDERER_GEOMETRY_TRIANGLE__

#include "base_geometry.hpp"
#include <core/struct/mesh.hpp>

namespace VTX::Renderer::Geometry
{
	class Triangle : public BaseGeometry
	{
	  public:
		inline static const Desc::Key		  VERTEX_LAYOUT_MESHES		 = "Meshes";
		inline static const Desc::Key		  GEOMETRY_TRIANGLES		 = "Triangles";
		inline static const Desc::Key		  INDEX_TRIANGLES			 = "Index.Triangles";
		inline static const Desc::Key		  INDIRECT_TRIANGLES		 = "Indirect.Triangles";
		inline static constexpr Desc::Binding BINDING_INDIRECT_TRIANGLES = 10;

		Triangle()
		{
			vertexLayout   = VERTEX_LAYOUT_MESHES;
			indiceBuffer   = INDEX_TRIANGLES;
			indirectBuffer = INDIRECT_TRIANGLES;
		}

		void registerMesh( const Desc::Handle p_handle, const Core::Struct::Mesh & p_mesh )
		{
			_addRange(
				p_handle, static_cast<Index>( p_mesh.indices.size() ), static_cast<Index>( p_mesh.vertices.size() )
			);
			_indices( p_handle ) = p_mesh.indices;
		}
	};
} // namespace VTX::Renderer::Geometry

#endif // !__VTX_RENDERER_GEOMETRY_TRIANGLE__
