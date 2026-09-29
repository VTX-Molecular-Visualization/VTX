#ifndef __VTX_RENDERER_LAYOUT_MESHES__
#define __VTX_RENDERER_LAYOUT_MESHES__

#include "base_layout.hpp"
#include <core/struct/mesh.hpp>
#include <string_view>

namespace VTX::Renderer::Layout
{
	class Meshes : public BaseLayout
	{
	  public:
		static constexpr std::string_view POSITIONS = "Meshes.Positions";
		static constexpr std::string_view NORMALS	= "Meshes.Normals";

		Meshes()
		{
			attributes.push_back( { Desc::Key { POSITIONS }, Desc::E_TYPE::VEC3F } );
			attributes.push_back( { Desc::Key { NORMALS }, Desc::E_TYPE::VEC3F } );
		}

		void registerMesh( const Desc::Handle p_handle, const Core::Struct::Mesh & p_mesh )
		{ add( p_handle, static_cast<Index>( p_mesh.vertices.size() ) ); }

		void upload(
			Context::ContextWrapper &  p_context,
			const Desc::Handle		   p_handle,
			const Core::Struct::Mesh & p_mesh
		)
		{
			p_context.setBuffer<Vec3f>( { Desc::Key { POSITIONS } }, p_mesh.vertices, offset( p_handle ) );
			p_context.setBuffer<Vec3f>( { Desc::Key { NORMALS } }, p_mesh.normals, offset( p_handle ) );
		}

	  protected:
		void _resize( Context::ContextWrapper & p_context, const Index p_size ) override
		{
			p_context.setBuffer<Vec3f>( { Desc::Key { POSITIONS } }, p_size );
			p_context.setBuffer<Vec3f>( { Desc::Key { NORMALS } }, p_size );
		}
	};
} // namespace VTX::Renderer::Layout

#endif // !__VTX_RENDERER_LAYOUT_MESHES__
