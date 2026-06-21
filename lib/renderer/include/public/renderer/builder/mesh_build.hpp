#ifndef __VTX_RENDERER_BUILDER_MESH_BUILD__
#define __VTX_RENDERER_BUILDER_MESH_BUILD__

#include "renderer/caches.hpp"
#include "renderer/geometry/geometries.hpp"
#include "renderer/layout/layouts.hpp"
#include "renderer/resource_handler.hpp"

namespace VTX::Renderer::Builder
{
	struct MeshRegistry
	{
		static void clear( Layouts & p_layouts, Geometries & p_geometries )
		{
			p_layouts.meshes.clear();
			p_geometries.triangles.clear();
		}

		static void registerMesh(
			ResourcePool<Cache::Mesh> & p_meshes,
			Geometries &				p_geometries,
			Layouts &					p_layouts,
			const Desc::Handle			p_handle
		)
		{
			const Core::Struct::Mesh & mesh = *p_meshes.get( p_handle ).data;
			p_layouts.meshes.registerMesh( p_handle, mesh );
			p_geometries.triangles.registerMesh( p_handle, mesh );
		}

		static void upload(
			Context::ContextWrapper &	p_context,
			ResourcePool<Cache::Mesh> & p_meshes,
			Geometries &				p_geometries,
			Layouts &					p_layouts
		)
		{
			p_layouts.meshes.resize( p_context );
			p_geometries.triangles.resize( p_context );

			for ( const auto entry : p_meshes.entries() )
			{
				const Core::Struct::Mesh & mesh = *entry.resource.data;
				p_layouts.meshes.upload( p_context, entry.handle, mesh );
				p_geometries.triangles.uploadIndexes( p_context, entry.handle );
			}
		}
	};
} // namespace VTX::Renderer::Builder

#endif // !__VTX_RENDERER_BUILDER_MESH_BUILD__
