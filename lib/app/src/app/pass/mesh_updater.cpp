#include "app/pass/mesh_updater.hpp"
#include "app/pending_updates.hpp"
#include "app/services.hpp"
#include <core/struct/mesh.hpp>
#include <renderer/renderer.hpp>
#include <util/exceptions.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	MeshUpdater::MeshUpdater()
	{
		auto & reg = REG();

		_meshLoadConnection = HUB().connect<Events::MeshLoad, &MeshUpdater::_onMeshLoad>( this );
		_transformUpdatedConnection
			= reg.on_update<Util::Math::Transform>().connect<&MeshUpdater::_onUpdateTransform>( this );
		_meshDestroyedConnection = reg.on_destroy<Core::Struct::Mesh>().connect<&MeshUpdater::_onDestroyMesh>( this );
	}

	void MeshUpdater::update( const float, const float )
	{
		auto & reg = REG();

		const bool meshChanged = not _meshAdded.empty() || not _meshRemoved.empty();

		Util::ExceptionCollector errors;

		// Add pending.
		while ( not _meshAdded.empty() )
		{
			const Entity meshEntity = _meshAdded.front();
			errors.run(
				"Mesh add failed",
				[ & ]()
				{
					assert( not _meshes.contains( meshEntity ) );

					const auto & mesh	   = reg.get<Core::Struct::Mesh>( meshEntity );
					const auto & transform = reg.get<Util::Math::Transform>( meshEntity );

					const Renderer::Desc::Handle meshHandle
						= RENDERER().addMesh( Renderer::Cache::Mesh { transform.computeMatrix(), &mesh } );

					_meshes.emplace( meshEntity, meshHandle );
				}
			);
			_meshAdded.erase( _meshAdded.begin() );
		}

		// Remove pending.
		while ( not _meshRemoved.empty() )
		{
			errors.run( "Mesh removal failed", [ & ]() { RENDERER().removeMesh( _meshRemoved.front() ); } );
			_meshRemoved.erase( _meshRemoved.begin() );
		}

		errors.run(
			"Mesh patch failed",
			[ & ]()
			{
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
			}
		);

		// Clear pending.
		_meshAdded.clear();
		_meshRemoved.clear();

		errors.rethrow();
	}

	void MeshUpdater::_onMeshLoad( const Events::MeshLoad & p_event )
	{
		assert( not _meshes.contains( p_event.mesh ) );

		_meshAdded.emplace_back( p_event.mesh );
		PENDING_UPDATES().mark( E_PENDING_UPDATE::PASSES );
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
		if ( std::erase( _meshAdded, p_entity ) > 0 )
		{
			return;
		}

		const auto it = _meshes.find( p_entity );
		if ( it == _meshes.end() )
		{
			return;
		}

		_meshRemoved.emplace_back( it->second );
		PENDING_UPDATES().mark( E_PENDING_UPDATE::PASSES );
		_meshes.erase( it );
	}
} // namespace VTX::App::Pass
