#include "scene.hpp"
#include "action/main.hpp"
#include "event/event_manager.hpp"
#include "math/transform.hpp"
#include "model/box.hpp"
#include "model/label.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"
#include <algorithm>

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

		MVC::MvcManager::get().deleteAllModels( _labels );
		_labels.clear();

		MVC::MvcManager::get().deleteAllModels( _boxes );
		_boxes.clear();

		while ( _paths.size() > 0 )
		{
			PathPtr const path = *_paths.begin();
			removePath( path );
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
		_itemOrder.emplace_back( p_molecule );
		_applySceneID( *p_molecule );

		_aabb.extend( p_molecule->getAABB() );
		p_molecule->referenceLinkedAABB( &_aabb );

		Model::Box * const box = MVC::MvcManager::get().instantiateModel<Model::Box, Math::AABB>( _aabb );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::BOX_CREATED, box ) );
		VTXApp::get().getScene().addBox( box );

		if ( p_sendEvent )
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_ADDED, p_molecule ) );

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::removeMolecule( MoleculePtr const p_molecule )
	{
		_molecules.erase( p_molecule );
		_itemOrder.erase( std::find( _itemOrder.begin(), _itemOrder.end(), p_molecule ) );
		_aabb.invalidate();
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_REMOVED, p_molecule ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::addPath( PathPtr const p_path )
	{
		_add( p_path, _paths );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::PATH_ADDED, p_path ) );
	}

	void Scene::removePath( PathPtr const p_path )
	{
		_remove( p_path, _paths );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::PATH_REMOVED, p_path ) );
	}

	void Scene::addMesh( MeshTrianglePtr const p_mesh )
	{
		_add( p_mesh, _meshes );
		_aabb.extend( p_mesh->getAABB() );
		p_mesh->referenceLinkedAABB( &_aabb );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_ADDED, p_mesh ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::removeMesh( MeshTrianglePtr const p_mesh )
	{
		_remove( p_mesh, _meshes );
		_aabb.invalidate();
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_REMOVED, p_mesh ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::addLabel( LabelPtr const p_label )
	{
		_add( p_label, _labels );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::LABEL_ADDED, p_label ) );
	}
	void Scene::removeLabel( LabelPtr const p_label )
	{
		_remove( p_label, _labels );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::LABEL_REMOVED, p_label ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::addBox( BoxPtr const p_box )
	{
		_add( p_box, _boxes );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::BOX_ADDED, p_box ) );
	}

	void Scene::removeBox( BoxPtr const p_box )
	{
		_remove( p_box, _boxes );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::BOX_REMOVED, p_box ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	const Generic::BaseSceneItem * const Scene::getItemAtPosition( const int p_index ) const
	{
		if ( 0 <= p_index && p_index < _itemOrder.size() )
			return _itemOrder[ p_index ];
		return nullptr;
	}
	int Scene::getItemPosition( const Generic::BaseSceneItem & p_item ) const
	{
		for ( int i = 0; i < _itemOrder.size(); i++ )
		{
			if ( &p_item == _itemOrder[ i ] )
				return i;
		}
		return -1;
	}
	void Scene::changeModelPosition( const Generic::BaseSceneItem & p_item, const int p_position )
	{
		const Generic::BaseSceneItem * itemPtr = &p_item;

		if ( _itemOrder[ p_position ] == itemPtr )
			return;

		bool changeHasStarted = false;

		for ( size_t i = 0; i < p_position; i++ )
		{
			if ( _itemOrder[ i ] == itemPtr )
				changeHasStarted = true;

			if ( changeHasStarted )
			{
				_itemOrder[ i ] = _itemOrder[ i + 1 ];
			}
		}

		if ( changeHasStarted )
		{
			_itemOrder[ p_position ] = itemPtr;
		}
		else
		{
			for ( int i = int( _itemOrder.size() - 1 ); i > p_position; i-- )
			{
				if ( _itemOrder[ i ] == itemPtr )
					changeHasStarted = true;

				if ( changeHasStarted )
					_itemOrder[ i ] = _itemOrder[ i - 1 ];
			}

			_itemOrder[ p_position ] = itemPtr;
		}

		VTX_EVENT( new Event::VTXEventPtr( Event::Global::SCENE_ITEM_INDEX_CHANGE, &p_item ) );
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

	void Scene::_applySceneID( Generic::BaseSceneItem & p_item )
	{
		if ( p_item.hasPersistentSceneID() )
		{
			_persistentIDCounter = _persistentIDCounter > p_item.getPersistentSceneID()
									   ? _persistentIDCounter
									   : p_item.getPersistentSceneID() + 1;
		}
		else
		{
			p_item.setPersistentSceneID( _persistentIDCounter++ );
		}
	}

	void Scene::_createDefaultPath()
	{
		Model::Path * const path = MVC::MvcManager::get().instantiateModel<Model::Path>();
		addPath( path );
	}

} // namespace VTX::Object3D
