#include "scene.hpp"
#include "action/main.hpp"
#include "camera_manager.hpp"
#include "math/transform.hpp"
#include "model/label.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/helper/base_helper.hpp"
#include <algorithm>

namespace VTX::Object3D
{
	Scene::Scene()
	{
		_cameraManager = new CameraManager();
		_createDefaultPath();
	}

	Scene::~Scene()
	{
		clear();
		delete _cameraManager;
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

		_helpers.clear();

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

	Camera &	   Scene::getCamera() { return *_cameraManager->getCamera(); }
	const Camera & Scene::getCamera() const { return *_cameraManager->getCamera(); }

	void Scene::addMolecule( MoleculePtr const p_molecule, const bool p_sendEvent )
	{
		p_molecule->init();
		p_molecule->print();
		_molecules.emplace( p_molecule, 0.f );
		_itemOrder.emplace_back( p_molecule );
		_applySceneID( *p_molecule );

		_aabb.extend( p_molecule->getAABB() );
		p_molecule->referenceLinkedAABB( &_aabb );

		if ( p_sendEvent )
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_ADDED, p_molecule ) );

		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::removeMolecule( MoleculePtr const p_molecule )
	{
		_remove( p_molecule, _molecules, Event::Global::MOLECULE_REMOVED, ModelCharacteristicsFlag::MOLECULE );
	}

	void Scene::addPath( PathPtr const p_path )
	{
		_add( p_path, _paths );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::PATH_ADDED, p_path ) );
	}

	void Scene::removePath( PathPtr const p_path )
	{
		_remove( p_path, _paths, Event::Global::PATH_REMOVED, ModelCharacteristicsFlag::NONE );
	}

	void Scene::addMesh( MeshTrianglePtr const p_mesh )
	{
		p_mesh->init();
		p_mesh->print();
		_add( p_mesh, _meshes );
		_aabb.extend( p_mesh->getAABB() );
		p_mesh->referenceLinkedAABB( &_aabb );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_ADDED, p_mesh ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::removeMesh( MeshTrianglePtr const p_mesh )
	{
		_remove( p_mesh, _meshes, Event::Global::MESH_REMOVED, ModelCharacteristicsFlag::MESH );
	}

	void Scene::addLabel( LabelPtr const p_label )
	{
		_add( p_label, _labels );
		VTX_EVENT( new Event::VTXEventPtr( Event::Global::LABEL_ADDED, p_label ) );
	}
	void Scene::removeLabel( LabelPtr const p_label )
	{
		_remove( p_label, _labels, Event::Global::LABEL_REMOVED, ModelCharacteristicsFlag::LABEL );
	}

	void Scene::addHelper( HelperPtr const p_helper )
	{
		p_helper->generate();
		_helpers.emplace_back( p_helper );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::removeHelper( HelperPtr const p_helper )
	{
		_helpers.erase( std::find( _helpers.begin(), _helpers.end(), p_helper ) );
		VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
	}

	void Scene::_updateGraphicMask() const { VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE; }

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

		VTX_EVENT( new Event::VTXEvent( Event::Global::SCENE_ITEM_INDEXES_CHANGE ) );
	}

	void Scene::changeModelsPosition( const std::vector<const Generic::BaseSceneItem *> & p_items,
									  const int											  p_position )
	{
		std::vector<const Generic::BaseSceneItem *> movedItems = std::vector<const Generic::BaseSceneItem *>();
		movedItems.resize( p_items.size() );

		size_t indexMovedItemsBeforePosition = 0;

		for ( size_t i = 0; i < p_position; i++ )
		{
			const bool hasToMoveItem = std::find( p_items.begin(), p_items.end(), _itemOrder[ i ] ) != p_items.end();

			if ( hasToMoveItem )
			{
				movedItems[ indexMovedItemsBeforePosition ] = _itemOrder[ i ];
				indexMovedItemsBeforePosition++;

				_itemOrder[ i ] = nullptr;
			}
			else if ( indexMovedItemsBeforePosition > 0 )
			{
				_itemOrder[ i - indexMovedItemsBeforePosition ] = _itemOrder[ i ];
			}
		}

		size_t itemMovedCounter = 0;

		for ( int i = int( _itemOrder.size() ) - 1; i > p_position; i-- )
		{
			const bool hasToMoveItem = std::find( p_items.begin(), p_items.end(), _itemOrder[ i ] ) != p_items.end();

			if ( hasToMoveItem )
			{
				const size_t movedItemsIndex  = movedItems.size() - 1 - itemMovedCounter;
				movedItems[ movedItemsIndex ] = _itemOrder[ i ];
				itemMovedCounter++;
				_itemOrder[ i ] = nullptr;
			}
			else if ( itemMovedCounter > 0 )
			{
				_itemOrder[ i + itemMovedCounter ] = _itemOrder[ i ];
			}
		}

		if ( p_position < _itemOrder.size() )
		{
			const bool hasToMoveItem
				= std::find( p_items.begin(), p_items.end(), _itemOrder[ p_position ] ) != p_items.end();
			if ( hasToMoveItem )
			{
				movedItems[ indexMovedItemsBeforePosition ] = _itemOrder[ p_position ];
				_itemOrder[ p_position ]					= nullptr;
			}
			else
			{
				_itemOrder[ p_position + itemMovedCounter ] = _itemOrder[ p_position ];
			}
		}

		for ( size_t i = 0; i < movedItems.size(); i++ )
		{
			_itemOrder[ p_position - indexMovedItemsBeforePosition + i ] = movedItems[ i ];
		}

		VTX_EVENT( new Event::VTXEvent( Event::Global::SCENE_ITEM_INDEXES_CHANGE ) );
	}

	void Scene::sortMoleculesBySceneIndex( std::vector<Model::Molecule *> & p_molecules ) const
	{
		for ( int i = 0; i < p_molecules.size(); i++ )
		{
			int smallerIndexInScene = getItemPosition( *p_molecules[ i ] );
			int indexInVector		= i;

			for ( int j = i + 1; j < p_molecules.size(); j++ )
			{
				const int currentIndexInScene = getItemPosition( *p_molecules[ j ] );
				if ( currentIndexInScene < smallerIndexInScene )
				{
					smallerIndexInScene = currentIndexInScene;
					indexInVector		= j;
				}
			}

			Model::Molecule * const tmp	 = p_molecules[ i ];
			p_molecules[ i ]			 = p_molecules[ indexInVector ];
			p_molecules[ indexInVector ] = tmp;
		}
	}

	const Object3D::Helper::AABB & Scene::getAABB()
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
