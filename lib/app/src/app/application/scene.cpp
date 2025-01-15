#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/representation/color_layout.hpp"
#include "app/component/representation/render_settings.hpp"
#include "app/component/representation/representation.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/updatable.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include "app/entity/camera.hpp"
#include <renderer/proxy/representation.hpp>

namespace VTX::App::Application
{
	Core::ECS::View<Component::Scene::SceneItemComponent> Scene::getAllSceneItems() const
	{
		return ECS_REGISTRY().findComponents<Component::Scene::SceneItemComponent>();
	}

	Scene::Scene()
	{
		auto cameraEntity = ECS_REGISTRY().createEntity<Entity::Camera>();
		_camera			  = &( ECS_REGISTRY().getComponent<Component::Render::Camera>( cameraEntity ) );

		_createDefaultPath();
		_createDefaultColorLayout();
		_createDefaultRenderSettings();
		_createDefaultRepresentation();
	}

	Scene::~Scene() {}

	Core::ECS::BaseEntity Scene::findItem( const FindItemFunction & p_findFunction ) const
	{
		auto view = getAllSceneItems();
		for ( const Core::ECS::BaseEntity & entity : view )
		{
			if ( p_findFunction( entity ) )
				return entity;
		}

		return Core::ECS::INVALID_ENTITY;
	}

	bool   Scene::isEmpty() const { return getItemCount() == 0; }
	size_t Scene::getItemCount() const { return getAllSceneItems().size(); }

	void Scene::referenceItem( Component::Scene::SceneItemComponent & p_item )
	{
		_itemIndexes.emplace_back( ECS_REGISTRY().getEntity( p_item ) );
		_aabb.invalidate();
		onSceneItemAdded( p_item );
	}

	const Core::ECS::BaseEntity Scene::getItem( const size_t p_index ) const
	{
		if ( p_index < 0 || p_index >= _itemIndexes.size() )
			return Core::ECS::INVALID_ENTITY;

		return _itemIndexes[ p_index ];
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

	void Scene::clear()
	{
		onSceneItemAdded.clear();

		_itemIndexes.clear();
		_itemIndexes.shrink_to_fit();

		ECS_REGISTRY().deleteAll<Component::Scene::SceneItemComponent>();
	}

	void Scene::reset()
	{
		clear();
		_createDefaultPath();
	}

	size_t Scene::getItemIndex( const Core::ECS::BaseEntity & p_entity ) const
	{
		for ( size_t i = 0; i < _itemIndexes.size(); i++ )
		{
			if ( _itemIndexes[ i ] == p_entity )
				return i;
		}

		return INVALID_INDEX;
	}
	void Scene::changeItemIndex( const Core::ECS::BaseEntity & p_entity, const size_t p_position )
	{
		if ( _itemIndexes[ p_position ] == p_entity )
			return;

		bool changeHasStarted = false;

		for ( size_t i = 0; i < p_position; i++ )
		{
			if ( _itemIndexes[ i ] == p_entity )
				changeHasStarted = true;

			if ( changeHasStarted )
			{
				_itemIndexes[ i ] = _itemIndexes[ i + 1 ];
			}
		}

		if ( changeHasStarted )
		{
			_itemIndexes[ p_position ] = p_entity;
		}
		else
		{
			for ( int i = int( _itemIndexes.size() - 1 ); i > p_position; i-- )
			{
				if ( _itemIndexes[ i ] == p_entity )
					changeHasStarted = true;

				if ( changeHasStarted )
					_itemIndexes[ i ] = _itemIndexes[ i - 1 ];
			}

			_itemIndexes[ p_position ] = p_entity;
		}

		// VTX_EVENT( Event::Global::SCENE_ITEM_INDEXES_CHANGE );
	}
	void Scene::changeItemsIndex( const std::vector<Core::ECS::BaseEntity> & p_items, const size_t p_position )
	{
		std::vector<Core::ECS::BaseEntity> movedItems = std::vector<Core::ECS::BaseEntity>();
		movedItems.resize( p_items.size(), Core::ECS::INVALID_ENTITY );

		size_t indexMovedItemsBeforePosition = 0;

		for ( size_t i = 0; i < p_position; i++ )
		{
			const bool hasToMoveItem = std::find( p_items.begin(), p_items.end(), _itemIndexes[ i ] ) != p_items.end();

			if ( hasToMoveItem )
			{
				movedItems[ indexMovedItemsBeforePosition ] = _itemIndexes[ i ];
				indexMovedItemsBeforePosition++;

				_itemIndexes[ i ] = Core::ECS::INVALID_ENTITY;
			}
			else if ( indexMovedItemsBeforePosition > 0 )
			{
				_itemIndexes[ i - indexMovedItemsBeforePosition ] = _itemIndexes[ i ];
			}
		}

		size_t itemMovedCounter = 0;

		for ( int i = int( _itemIndexes.size() ) - 1; i > p_position; i-- )
		{
			const bool hasToMoveItem = std::find( p_items.begin(), p_items.end(), _itemIndexes[ i ] ) != p_items.end();

			if ( hasToMoveItem )
			{
				const size_t movedItemsIndex  = movedItems.size() - 1 - itemMovedCounter;
				movedItems[ movedItemsIndex ] = _itemIndexes[ i ];
				itemMovedCounter++;
				_itemIndexes[ i ] = Core::ECS::INVALID_ENTITY;
			}
			else if ( itemMovedCounter > 0 )
			{
				_itemIndexes[ i + itemMovedCounter ] = _itemIndexes[ i ];
			}
		}

		if ( p_position < _itemIndexes.size() )
		{
			const bool hasToMoveItem
				= std::find( p_items.begin(), p_items.end(), _itemIndexes[ p_position ] ) != p_items.end();
			if ( hasToMoveItem )
			{
				movedItems[ indexMovedItemsBeforePosition ] = _itemIndexes[ p_position ];
				_itemIndexes[ p_position ]					= Core::ECS::INVALID_ENTITY;
			}
			else
			{
				_itemIndexes[ p_position + itemMovedCounter ] = _itemIndexes[ p_position ];
			}
		}

		for ( size_t i = 0; i < movedItems.size(); i++ )
		{
			_itemIndexes[ p_position - indexMovedItemsBeforePosition + i ] = movedItems[ i ];
		}

		// VTX_EVENT( Event::Global::SCENE_ITEM_INDEXES_CHANGE );
	}

	void Scene::sortItemsBySceneIndex( std::vector<Core::ECS::BaseEntity> & p_items ) const
	{
		for ( size_t i = 0; i < p_items.size(); i++ )
		{
			size_t smallerIndexInScene = getItemIndex( p_items[ i ] );
			size_t indexInVector	   = i;

			for ( size_t j = i + 1; j < p_items.size(); j++ )
			{
				const size_t currentIndexInScene = getItemIndex( p_items[ j ] );
				if ( currentIndexInScene < smallerIndexInScene )
				{
					smallerIndexInScene = currentIndexInScene;
					indexInVector		= j;
				}
			}

			std::swap( p_items[ i ], p_items[ indexInVector ] );
		}
	}

	const Util::Math::AABB & Scene::getAABB()
	{
		if ( not _aabb.isValid() )
		{
			_computeAABB();
		}

		return _aabb;
	}
	void Scene::_computeAABB()
	{
		const Core::ECS::View view
			= ECS_REGISTRY().findComponents<Component::Scene::SceneItemComponent, Component::Scene::AABB>();

		_aabb.invalidate();

		for ( const Core::ECS::BaseEntity entity : view )
		{
			Component::Scene::AABB & aabbComponent = ECS_REGISTRY().getComponent<Component::Scene::AABB>( entity );
			_aabb.extend( aabbComponent.getWorldAABB() );
		}
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

	void Scene::_createDefaultColorLayout()
	{
		auto & comp
			= ECS_REGISTRY().addComponent<Component::Representation::ColorLayout>( ECS_REGISTRY().getEntity( *this ) );
		comp.setupProxy();
	}

	void Scene::_createDefaultRenderSettings()
	{
		auto & comp
			= ECS_REGISTRY().addComponent<Component::Representation::RenderSettings>( ECS_REGISTRY().getEntity( *this )
			);
		comp.setupProxy();
	}

	void Scene::_createDefaultRepresentation()
	{
		auto & comp
			= ECS_REGISTRY().addComponent<Component::Representation::Representation>( ECS_REGISTRY().getEntity( *this )
			);
		comp.setupProxy();
	}
} // namespace VTX::App::Application
