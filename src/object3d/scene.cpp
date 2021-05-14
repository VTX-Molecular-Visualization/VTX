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
		MVC::MvcManager::get().deleteAllModels( _paths );
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

	void Scene::addPath( PathPtr const p_path ) { _paths.emplace_back( p_path ); }

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

		const Vec3f & autoRotationSpeed = VTX_SETTING().getAutoRotationSpeed();
		// Auto rotate.
		if ( autoRotationSpeed != VEC3F_ZERO )
		{
			for ( const PairMoleculePtrFloat & pair : _molecules )
			{
				pair.first->rotate( p_deltaTime * autoRotationSpeed.x, VEC3F_X );
				pair.first->rotate( p_deltaTime * autoRotationSpeed.y, VEC3F_Y );
				pair.first->rotate( p_deltaTime * autoRotationSpeed.z, VEC3F_Z );
			}

			for ( const MeshTrianglePtr & mesh : _meshes )
			{
				mesh->rotate( p_deltaTime * autoRotationSpeed.x, VEC3F_X );
				mesh->rotate( p_deltaTime * autoRotationSpeed.y, VEC3F_Y );
				mesh->rotate( p_deltaTime * autoRotationSpeed.z, VEC3F_Z );
			}
		}
	}

	void Scene::_createDefaultPath()
	{
		Model::Path * const path = MVC::MvcManager::get().instantiateModel<Model::Path>();
		addPath( path );
	}

} // namespace VTX::Object3D
