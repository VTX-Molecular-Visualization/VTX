#include "app/pass/mesh_updater.hpp"
#include "app/services.hpp"
#include <core/struct/mesh.hpp>
#include <renderer/renderer.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	MeshUpdater::MeshUpdater()
	{
		auto & reg = REG();

		HUB().connect<Events::MeshLoad, &MeshUpdater::_onMeshLoad>( this );
		reg.on_update<Util::Math::Transform>().connect<&MeshUpdater::_onUpdateTransform>( this );
		reg.on_destroy<Core::Struct::Mesh>().connect<&MeshUpdater::_onDestroyMesh>( this );
	}

	void MeshUpdater::update( const float, const float )
	{
		auto & reg = REG();

		const bool meshChanged = not _meshAdded.empty() || not _meshRemoved.empty();

		// Add pending.
		for ( const Entity meshEntity : _meshAdded )
		{
			assert( not _meshes.contains( meshEntity ) );

			const auto & mesh	   = reg.get<Core::Struct::Mesh>( meshEntity );
			const auto & transform = reg.get<Util::Math::Transform>( meshEntity );

			const Renderer::Desc::Handle meshHandle
				= RENDERER().addMesh( Renderer::Cache::Mesh { transform.computeMatrix(), &mesh } );

			_meshes.emplace( meshEntity, meshHandle );
		}

		// Remove pending.
		for ( const Renderer::Desc::Handle mesh : _meshRemoved )
		{
			RENDERER().removeMesh( mesh );
		}

		// Patch because renderer use views to raw data.
		// entt components are not guaranteed to be contiguous in memory.
		// So ptr can dangle after add/remove.
		if ( meshChanged )
		{
			for ( const auto & pair : _meshes )
			{
				RENDERER().patchMesh( pair.second, reg.get<Core::Struct::Mesh>( pair.first ) );
			}
		}

		// Clear pending.
		_meshAdded.clear();
		_meshRemoved.clear();
	}

	void MeshUpdater::_onMeshLoad( const Events::MeshLoad & p_event )
	{
		assert( not _meshes.contains( p_event.mesh ) );

		_meshAdded.emplace_back( p_event.mesh );
	}

	void MeshUpdater::_onUpdateTransform( Registry & p_registry, const Entity p_entity )
	{
		if ( _meshes.contains( p_entity ) )
		{
			const auto & transform = p_registry.get<Util::Math::Transform>( p_entity );
			RENDERER().setMeshTransform( _meshes[ p_entity ], transform.computeMatrix() );
		}
	}

	void MeshUpdater::_onDestroyMesh( Registry &, const Entity p_entity )
	{
		assert( _meshes.contains( p_entity ) );

		_meshRemoved.emplace_back( _meshes[ p_entity ] );
		_meshes.erase( p_entity );
	}
} // namespace VTX::App::Pass
