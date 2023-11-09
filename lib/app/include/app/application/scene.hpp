#ifndef __VTX_APP_APPLICATION_SCENE__
#define __VTX_APP_APPLICATION_SCENE__

#include "app/application/ecs/registry_manager.hpp"
#include "app/component/render/_fwd.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/core/callback_event.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"
#include <concepts>
#include <string>
#include <util/math/aabb.hpp>

namespace VTX::App::Application
{
	template<typename T>
	concept SceneItem = requires( T sceneItem ) { std::derived_from<T, Component::Scene::SceneItemComponent>; };

	class Scene
	{
	  public:
		Scene();
		~Scene();

		Core::ECS::View<Component::Scene::SceneItemComponent> getAllSceneItems() const;

		template<SceneItem T>
		Core::ECS::View<Component::Scene::SceneItemComponent, T> getAllSceneItemsOfType() const
		{
			return VTXApp::MAIN_REGISTRY().getComponents<Component::Scene::SceneItemComponent, T>();
		}

		void referenceItem( Component::Scene::SceneItemComponent & p_item );

		const Core::ECS::BaseEntity getItem( const size_t p_index ) const;
		const Core::ECS::BaseEntity getItem( const std::string & p_name ) const;

		template<typename C>
		const C & getComponentByName( const std::string & p_name ) const
		{
			const Core::ECS::BaseEntity entity = getItem( p_name );
			return VTXApp::MAIN_REGISTRY().getComponent<C>( entity );
		}

		template<typename C>
		C & getComponentByName( const std::string & p_name )
		{
			const Core::ECS::BaseEntity entity = getItem( p_name );
			return VTXApp::MAIN_REGISTRY().getComponent<C>( entity );
		}

		bool   isEmpty() const;
		size_t getItemCount() const;

		virtual void update( const float & );

		void clear();
		void reset();

		const Util::Math::AABB & getAABB();

		size_t getItemIndex( const Core::ECS::BaseEntity & p_item ) const;
		void   changeItemIndex( const Core::ECS::BaseEntity & p_item, const size_t p_index );
		void   changeItemsIndex( const std::vector<Core::ECS::BaseEntity> & p_items, const size_t p_position );

		template<Core::ECS::ECS_Component C>
		size_t getItemIndex( const C & p_item ) const
		{
			const Core::ECS::BaseEntity & entity = MAIN_REGISTRY().getEntity( p_item );
			return getItemIndex( entity );
		}
		template<Core::ECS::ECS_Component C>
		void changeItemIndex( const C & p_item, const size_t p_index )
		{
			const Core::ECS::BaseEntity & entity = MAIN_REGISTRY().getEntity( p_item );
			changeItemIndex( entity, p_index );
		}
		template<Core::ECS::ECS_Component C>
		void changeItemsIndex( const std::vector<const C *> & p_items, const size_t p_position )
		{
			std::vector<Core::ECS::BaseEntity> p_itemEntities = std::vector<Core::ECS::BaseEntity>();
			p_itemEntities.reserve( p_items.size() );
			for ( const Component::Scene::SceneItemComponent * const itemPtr : p_items )
			{
				const Core::ECS::BaseEntity itemEntity = MAIN_REGISTRY().getEntity( *itemPtr );
				p_itemEntities.emplace_back( itemEntity );
			}

			changeItemsIndex( p_itemEntities, p_position );
		}

		void sortItemsBySceneIndex( std::vector<Core::ECS::BaseEntity> & p_items ) const;

		template<Core::ECS::ECS_Component C>
		void sortItemsBySceneIndex( std::vector<C *> & p_items ) const
		{
			for ( size_t i = 0; i < p_items.size(); i++ )
			{
				size_t smallerIndexInScene = getItemIndex( *p_items[ i ] );
				size_t indexInVector	   = i;

				for ( size_t j = i + 1; j < p_items.size(); j++ )
				{
					const size_t currentIndexInScene = getItemIndex( *p_items[ j ] );
					if ( currentIndexInScene < smallerIndexInScene )
					{
						smallerIndexInScene = currentIndexInScene;
						indexInVector		= j;
					}
				}

				std::swap( p_items[ i ], p_items[ indexInVector ] );
			}
		}

		// Callbacks
		Core::CallbackRegister<Component::Scene::SceneItemComponent &> & onSceneItemAddedCallback()
		{
			return _onSceneItemAddedCallback;
		}

	  private:
		int _persistentIDCounter = 0;

		void _computeAABB();
		void _createDefaultPath();

	  private:
		Component::Render::Camera *		   _camera		= nullptr;
		Util::Math::AABB				   _aabb		= Util::Math::AABB();
		std::vector<Core::ECS::BaseEntity> _itemIndexes = std::vector<Core::ECS::BaseEntity>();

		void _onSceneItemIsConstruct( Component::Scene::SceneItemComponent & p_sceneItemComponent );

		void _applySceneID( Component::Scene::SceneItemComponent & p_item );

		// Callbacks
		Core::CallbackEmitter<Component::Scene::SceneItemComponent &> _onSceneItemAddedCallback
			= Core::CallbackEmitter<Component::Scene::SceneItemComponent &>();
	};
} // namespace VTX::App::Application
#endif
