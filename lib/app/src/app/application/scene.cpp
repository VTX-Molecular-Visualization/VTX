#include "app/application/scene.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/updatable.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/entity/all_entities.hpp"

namespace VTX::App::Application
{
	Core::ECS::View<Component::Scene::SceneItemComponent> Scene::getAllSceneItems() const
	{
		return MAIN_REGISTRY().getComponents<Component::Scene::SceneItemComponent>();
	}

	Scene::Scene()
	{
		App::Core::ECS::BaseEntity cameraEntity = ECS::EntityDirector::build( Entity::CAMERA_ENTITY_ID );
		_camera = &( MAIN_REGISTRY().getComponent<Component::Render::Camera>( cameraEntity ) );

		_createDefaultPath();
	}

	Scene::~Scene() {}

	bool   Scene::isEmpty() const { return getItemCount() == 0; }
	size_t Scene::getItemCount() const { return getAllSceneItems().size(); }

	void Scene::referenceItem( Component::Scene::SceneItemComponent & p_item )
	{
		_onSceneItemAddedCallback.emit( p_item );
	}

	const Core::ECS::BaseEntity Scene::getItem( const size_t p_index ) const
	{
		size_t count = 0;

		for ( const Core::ECS::BaseEntity entity : getAllSceneItems() )
		{
			if ( count == p_index )
				return entity;

			count++;
		}

		return Core::ECS::INVALID_ENTITY;
	}
	const Core::ECS::BaseEntity Scene::getItem( const std::string & p_name ) const
	{
		const Core::ECS::View view = getAllSceneItems();

		for ( const Core::ECS::BaseEntity entity : view )
		{
			const Component::Scene::SceneItemComponent & sceneItem
				= view.getComponent<Component::Scene::SceneItemComponent>( entity );

			if ( sceneItem.getName() == p_name )
				return entity;
		}

		return Core::ECS::INVALID_ENTITY;
	}

	void Scene::clear() { MAIN_REGISTRY().deleteAll<Component::Scene::SceneItemComponent>(); }

	void Scene::reset()
	{
		clear();
		_createDefaultPath();
	}

	void Scene::changeItemIndex( const Component::Scene::SceneItemComponent & p_item, const int p_position )
	{
		// const ECS::Component::SceneItemComponent * itemPtr = &p_item;

		// if ( _itemOrder[ p_position ] == itemPtr )
		//	return;

		// bool changeHasStarted = false;

		// for ( size_t i = 0; i < p_position; i++ )
		//{
		//	if ( _itemOrder[ i ] == itemPtr )
		//		changeHasStarted = true;

		//	if ( changeHasStarted )
		//	{
		//		_itemOrder[ i ] = _itemOrder[ i + 1 ];
		//	}
		//}

		// if ( changeHasStarted )
		//{
		//	_itemOrder[ p_position ] = itemPtr;
		// }
		// else
		//{
		//	for ( int i = int( _itemOrder.size() - 1 ); i > p_position; i-- )
		//	{
		//		if ( _itemOrder[ i ] == itemPtr )
		//			changeHasStarted = true;

		//		if ( changeHasStarted )
		//			_itemOrder[ i ] = _itemOrder[ i - 1 ];
		//	}

		//	_itemOrder[ p_position ] = itemPtr;
		//}

		// VTX_EVENT( Event::Global::SCENE_ITEM_INDEXES_CHANGE );
	}
	void Scene::changeItemsIndex( const std::vector<const Component::Scene::SceneItemComponent *> & p_items,
								  const int															p_position )
	{
		// std::vector<const Core::Scene::BaseSceneItem *> movedItems = std::vector<const Core::Scene::BaseSceneItem
		// *>(); movedItems.resize( p_items.size() );

		// size_t indexMovedItemsBeforePosition = 0;

		// for ( size_t i = 0; i < p_position; i++ )
		//{
		//	const bool hasToMoveItem = std::find( p_items.begin(), p_items.end(), _itemOrder[ i ] ) != p_items.end();

		//	if ( hasToMoveItem )
		//	{
		//		movedItems[ indexMovedItemsBeforePosition ] = _itemOrder[ i ];
		//		indexMovedItemsBeforePosition++;

		//		_itemOrder[ i ] = nullptr;
		//	}
		//	else if ( indexMovedItemsBeforePosition > 0 )
		//	{
		//		_itemOrder[ i - indexMovedItemsBeforePosition ] = _itemOrder[ i ];
		//	}
		//}

		// size_t itemMovedCounter = 0;

		// for ( int i = int( _itemOrder.size() ) - 1; i > p_position; i-- )
		//{
		//	const bool hasToMoveItem = std::find( p_items.begin(), p_items.end(), _itemOrder[ i ] ) != p_items.end();

		//	if ( hasToMoveItem )
		//	{
		//		const size_t movedItemsIndex  = movedItems.size() - 1 - itemMovedCounter;
		//		movedItems[ movedItemsIndex ] = _itemOrder[ i ];
		//		itemMovedCounter++;
		//		_itemOrder[ i ] = nullptr;
		//	}
		//	else if ( itemMovedCounter > 0 )
		//	{
		//		_itemOrder[ i + itemMovedCounter ] = _itemOrder[ i ];
		//	}
		//}

		// if ( p_position < _itemOrder.size() )
		//{
		//	const bool hasToMoveItem
		//		= std::find( p_items.begin(), p_items.end(), _itemOrder[ p_position ] ) != p_items.end();
		//	if ( hasToMoveItem )
		//	{
		//		movedItems[ indexMovedItemsBeforePosition ] = _itemOrder[ p_position ];
		//		_itemOrder[ p_position ]					= nullptr;
		//	}
		//	else
		//	{
		//		_itemOrder[ p_position + itemMovedCounter ] = _itemOrder[ p_position ];
		//	}
		// }

		// for ( size_t i = 0; i < movedItems.size(); i++ )
		//{
		//	_itemOrder[ p_position - indexMovedItemsBeforePosition + i ] = movedItems[ i ];
		// }

		// VTX_EVENT( Event::Global::SCENE_ITEM_INDEXES_CHANGE );
	}
	void Scene::sortItemsBySceneIndex( std::vector<Component::Scene::SceneItemComponent *> & p_molecules ) const
	{
		// for ( int i = 0; i < p_molecules.size(); i++ )
		//{
		//	int smallerIndexInScene = getItemPosition( *p_molecules[ i ] );
		//	int indexInVector		= i;

		//	for ( int j = i + 1; j < p_molecules.size(); j++ )
		//	{
		//		const int currentIndexInScene = getItemPosition( *p_molecules[ j ] );
		//		if ( currentIndexInScene < smallerIndexInScene )
		//		{
		//			smallerIndexInScene = currentIndexInScene;
		//			indexInVector		= j;
		//		}
		//	}

		//	Component::Chemistry::Molecule * const tmp = p_molecules[ i ];
		//	p_molecules[ i ]						   = p_molecules[ indexInVector ];
		//	p_molecules[ indexInVector ]			   = tmp;
		//}
	}

	const Util::Math::AABB & Scene::getAABB()
	{
		if ( !_aabb.isValid() )
			_computeAABB();

		return _aabb;
	}
	void Scene::_computeAABB()
	{
		const Core::ECS::View view
			= MAIN_REGISTRY().getComponents<Component::Scene::SceneItemComponent, Component::Scene::AABB>();

		_aabb.invalidate();

		for ( const Core::ECS::BaseEntity entity : view )
		{
			const Component::Scene::AABB & aabbComponent = view.getComponent<Component::Scene::AABB>( entity );
			_aabb.extend( aabbComponent.getWorldAABB() );
		}
	}

	void Scene::update( const float & p_deltaTime )
	{
		// TOCHECK: do that in state or in scene?
		// (let that here instead of doing the exact same things in all states for the moment)

		const Core::ECS::View view
			= MAIN_REGISTRY().getComponents<Component::Scene::SceneItemComponent, Component::Scene::Updatable>();

		for ( const Core::ECS::BaseEntity entity : view )
		{
			const Component::Scene::Updatable & updatableComponent
				= view.getComponent<Component::Scene::Updatable>( entity );
			updatableComponent.update( p_deltaTime );
		}

		// Dynamic.
		// for ( PairMoleculePtrFloat & pair : _molecules )
		//{
		//	MoleculePtr const molecule = pair.first;
		//	molecule->updateTrajectory( p_deltaTime );
		//}

		// for ( const PairMoleculePtrFloat & pair : _molecules )
		//{
		//	if ( pair.first->isAutoRotationPlaying() )
		//	{
		//		pair.first->rotate( p_deltaTime * pair.first->getAutoRotationVector().x, VEC3F_X );
		//		pair.first->rotate( p_deltaTime * pair.first->getAutoRotationVector().y, VEC3F_Y );
		//		pair.first->rotate( p_deltaTime * pair.first->getAutoRotationVector().z, VEC3F_Z );
		//		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
		//	}
		// }

		// for ( const MeshTrianglePtr & mesh : _meshes )
		//{
		//	if ( mesh->isAutoRotationPlaying() )
		//	{
		//		mesh->rotate( p_deltaTime * mesh->getAutoRotationVector().x, VEC3F_X );
		//		mesh->rotate( p_deltaTime * mesh->getAutoRotationVector().y, VEC3F_Y );
		//		mesh->rotate( p_deltaTime * mesh->getAutoRotationVector().z, VEC3F_Z );
		//		App::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
		//	}
		// }
	}

	void Scene::_applySceneID( Component::Scene::SceneItemComponent & p_item )
	{
		// if ( p_item.hasPersistentSceneID() )
		//{
		//	_persistentIDCounter = _persistentIDCounter > p_item.getPersistentSceneID()
		//							   ? _persistentIDCounter
		//							   : p_item.getPersistentSceneID() + 1;
		// }
		// else
		//{
		//	p_item.setPersistentSceneID( _persistentIDCounter++ );
		// }
	}

	void Scene::_createDefaultPath()
	{
		// Component::Video::Path * const path = VTX::MVC_MANAGER().instantiateModel<Component::Video::Path>();
		//_defaultPath						= path;

		// addPath( path );
	}

} // namespace VTX::App::Application
