#include "scene.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "math/transform.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX::Object3D
{
	Scene::Scene()
	{
		_camera = new Camera();
		_createDefaultPath();
	}

	Scene::~Scene()
	{
		clear();
		delete _camera;
	}

	bool Scene::isEmpty() const
	{
		return _molecules.size() == 0 && _meshes.size() == 0
			   && ( _paths.size() <= 0 || _paths[ 0 ]->getViewpoints().size() == 0 );
	}

	void Scene::clear()
	{
		while ( _molecules.size() > 0 )
		{
			MoleculePtr const molecule = _molecules.begin()->first;
			removeMolecule( molecule );
			MVC::MvcManager::get().deleteModel( molecule );
		}

		_molecules.clear();

		MVC::MvcManager::get().deleteAllModels( _meshes );
		_meshes.clear();
		
		while ( _paths.size() > 0 )
		{
			PathPtr const path = *_paths.begin();
			removePath( path);
			MVC::MvcManager::get().deleteModel( path );
		}

		_paths.clear();
	}

	void Scene::reset()
	{
		clear();
		_createDefaultPath();
	}

	void Scene::addMolecule( MoleculePtr const p_molecule, const bool p_sendEvent )
	{
		_molecules.emplace( p_molecule, 0.f );
		_aabb.extend( p_molecule->getAABB() );
		p_molecule->referenceLinkedAABB( &_aabb );

		if ( p_sendEvent )
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_ADDED, p_molecule ) );

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::removeMolecule( MoleculePtr const p_molecule )
	{
		_molecules.erase( p_molecule );
		_aabb.invalidate();
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_REMOVED, p_molecule ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::addPath( PathPtr const p_path )
	{
		_paths.emplace_back( p_path );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::PATH_ADDED, p_path ) );
	}

	void Scene::removePath( PathPtr const p_path )
	{
		_paths.erase( std::find( _paths.begin(), _paths.end(), p_path ) );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::PATH_REMOVED, p_path ) );
	}

	void Scene::addMesh( MeshTrianglePtr const p_mesh )
	{
		_meshes.emplace_back( p_mesh );
		_aabb.extend( p_mesh->getAABB() );
		p_mesh->referenceLinkedAABB( &_aabb );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_ADDED, p_mesh ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::removeMesh( MeshTrianglePtr const p_mesh )
	{
		_meshes.erase( std::find( _meshes.begin(), _meshes.end(), p_mesh ) );
		_aabb.invalidate();
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_REMOVED, p_mesh ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	const Math::AABB & Scene::getAABB()
	{
		if ( !_aabb.isValid() )
			_computeAABB();

		return _aabb;
	}
	void Scene::_computeAABB()
	{
		_aabb.invalidate();
		for ( const PairMoleculePtrFloat & mol : _molecules )
		{
			_aabb.extend( mol.first->getWorldAABB() );
		}
		for ( const MeshTrianglePtr & mesh : _meshes )
		{
			_aabb.extend( mesh->getWorldAABB() );
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
			molecule->updateTrajectory( p_deltaTime );
		}

		for ( const PairMoleculePtrFloat & pair : _molecules )
		{
			if ( pair.first->isAutoRotationPlaying() )
			{
				pair.first->rotate( p_deltaTime * pair.first->getAutoRotationVector().x, VEC3F_X );
				pair.first->rotate( p_deltaTime * pair.first->getAutoRotationVector().y, VEC3F_Y );
				pair.first->rotate( p_deltaTime * pair.first->getAutoRotationVector().z, VEC3F_Z );
				VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}
		}

		for ( const MeshTrianglePtr & mesh : _meshes )
		{
			if ( mesh->isAutoRotationPlaying() )
			{
				mesh->rotate( p_deltaTime * mesh->getAutoRotationVector().x, VEC3F_X );
				mesh->rotate( p_deltaTime * mesh->getAutoRotationVector().y, VEC3F_Y );
				mesh->rotate( p_deltaTime * mesh->getAutoRotationVector().z, VEC3F_Z );
				VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
			}
		}
	}

	void Scene::_createDefaultPath()
	{
		Model::Path * const path = MVC::MvcManager::get().instantiateModel<Model::Path>();
		addPath( path );
	}

} // namespace VTX::Object3D
