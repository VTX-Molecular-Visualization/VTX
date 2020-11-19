#include "scene.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "generic/factory.hpp"
#include "math/transform.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX
{
	namespace Object3D
	{
		Scene::Scene()
		{
			Model::Path * path = MVC::MvcManager::get().instantiate<Model::Path>();
			addPath( path );
		}

		void Scene::clear()
		{
			for ( const PairMoleculePtrFloat & mol : _molecules )
			{
				MVC::MvcManager::get().deleteModel( mol.first );
			}
			_molecules.clear();

			MVC::MvcManager::get().deleteAllModels( _meshes );
			MVC::MvcManager::get().deleteAllModels( _paths );
		}

		void Scene::addMolecule( MoleculePtr const p_molecule )
		{
			_molecules.emplace( p_molecule, 0.f );
			_aabb.extend( p_molecule->getAABB() );
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_ADDED, p_molecule ) );
		}

		void Scene::removeMolecule( MoleculePtr const p_molecule )
		{
			_molecules.erase( p_molecule );
			_computeAABB();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_REMOVED, p_molecule ) );
		}

		void Scene::addPath( PathPtr const p_path ) { _paths.emplace_back( p_path ); }

		void Scene::addMesh( MeshTrianglePtr const p_mesh )
		{
			_meshes.emplace_back( p_mesh );
			_aabb.extend( p_mesh->getAABB() );
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_ADDED, p_mesh ) );
		}

		void Scene::removeMesh( MeshTrianglePtr const p_mesh )
		{
			_meshes.erase( std::find( _meshes.begin(), _meshes.end(), p_mesh ) );
			_computeAABB();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_REMOVED, p_mesh ) );
		}

		void Scene::_computeAABB()
		{
			_aabb.invalidate();
			for ( const PairMoleculePtrFloat & mol : _molecules )
			{
				_aabb.extend( mol.first->getAABB() );
			}
			for ( const MeshTrianglePtr & mesh : _meshes )
			{
				_aabb.extend( mesh->getAABB() );
			}
		}

		void Scene::update( const double & p_deltaTime )
		{
			// TOCHECK: do that in state or in scene?
			// (let that here instead of doing the exact same things in all states for the moment)

			// Dynamic.
			for ( PairMoleculePtrFloat & pair : _molecules )
			{
				MoleculePtr const molecule = pair.first;
				float			  time	   = pair.second;

				uint frameCount = molecule->getFrameCount();
				if ( molecule->isPlaying() == false || frameCount < 2 )
				{
					continue;
				}

				const uint frame = molecule->getFrame();
				const uint fps	 = molecule->getFPS();

				uint nextFrame = frame;

				if ( fps == 0u )
				{
					molecule->setFrame( ++nextFrame % frameCount );
				}
				else
				{
					time += float( p_deltaTime );
					float offset = 1.f / float( fps );
					while ( time >= offset )
					{
						nextFrame++;
						time -= offset;
					}

					pair.second = time;
					if ( nextFrame != frame )
					{
						molecule->setFrame( nextFrame % frameCount );
					}
				}
			}

			// Auto rotate.
			if ( VTX_SETTING().autoRotationSpeed != VEC3F_ZERO )
			{
				for ( const PairMoleculePtrFloat & pair : _molecules )
				{
					pair.first->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.x, VEC3F_X );
					pair.first->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.y, VEC3F_Y );
					pair.first->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.z, VEC3F_Z );
				}

				for ( const MeshTrianglePtr & mesh : _meshes )
				{
					mesh->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.x, VEC3F_X );
					mesh->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.y, VEC3F_Y );
					mesh->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.z, VEC3F_Z );
				}
			}
		} // namespace Object3D

	} // namespace Object3D
} // namespace VTX
