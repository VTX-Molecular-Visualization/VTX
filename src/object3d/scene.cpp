#include "scene.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "math/transform.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX
{
	namespace Object3D
	{
		Scene::Scene()
		{
			_camera					 = new Camera();
			Model::Path * const path = MVC::MvcManager::get().instantiateModel<Model::Path>();
			addPath( path );
		}

		Scene::~Scene()
		{
			clear();
			delete _camera;
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
			VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
		}

		void Scene::removeMolecule( MoleculePtr const p_molecule )
		{
			_molecules.erase( p_molecule );
			_computeAABB();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_REMOVED, p_molecule ) );
			VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
		}

		void Scene::addPath( PathPtr const p_path ) { _paths.emplace_back( p_path ); }

		void Scene::addMesh( MeshTrianglePtr const p_mesh )
		{
			_meshes.emplace_back( p_mesh );
			_aabb.extend( p_mesh->getAABB() );
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_ADDED, p_mesh ) );
			VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
		}

		void Scene::removeMesh( MeshTrianglePtr const p_mesh )
		{
			_meshes.erase( std::find( _meshes.begin(), _meshes.end(), p_mesh ) );
			_computeAABB();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_REMOVED, p_mesh ) );
			VTXApp::get().MASK |= VTX_MASK_SCENE_UPDATED;
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

		void Scene::update( const float & p_deltaTime )
		{
			// TOCHECK: do that in state or in scene?
			// (let that here instead of doing the exact same things in all states for the moment)

			// Dynamic.
			for ( PairMoleculePtrFloat & pair : _molecules )
			{
				MoleculePtr const molecule = pair.first;
				float			  time	   = pair.second;

				const uint frameCount = molecule->getFrameCount();
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
					time += p_deltaTime;
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
					pair.first->rotate( p_deltaTime * VTX_SETTING().autoRotationSpeed.x, VEC3F_X );
					pair.first->rotate( p_deltaTime * VTX_SETTING().autoRotationSpeed.y, VEC3F_Y );
					pair.first->rotate( p_deltaTime * VTX_SETTING().autoRotationSpeed.z, VEC3F_Z );
				}

				for ( const MeshTrianglePtr & mesh : _meshes )
				{
					mesh->rotate( p_deltaTime * VTX_SETTING().autoRotationSpeed.x, VEC3F_X );
					mesh->rotate( p_deltaTime * VTX_SETTING().autoRotationSpeed.y, VEC3F_Y );
					mesh->rotate( p_deltaTime * VTX_SETTING().autoRotationSpeed.z, VEC3F_Z );
				}
			}
		} // namespace Object3D

	} // namespace Object3D
} // namespace VTX
