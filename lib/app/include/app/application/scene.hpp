#ifndef __VTX_APP_APPLICATION_SCENE__
#define __VTX_APP_APPLICATION_SCENE__

#include "app/application/system/ecs_system.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/vtx_app.hpp"
#include <concepts>
#include <functional>
#include <string>
#include <util/callback.hpp>
#include <util/math/aabb.hpp>

namespace VTX::App::Component::Representation
{
	class ColorLayout;
}

namespace VTX::App::Component::Render
{
	class Camera;
} // namespace VTX::App::Component::Render

namespace VTX::App::Application
{
	template<typename T>
	concept SceneItem = requires( T sceneItem ) { std::derived_from<T, Component::Scene::SceneItemComponent>; };

	class Scene : public Core::ECS::BaseComponent
	{
	  public:
		using FindItemFunction = std::function<bool( const Core::ECS::BaseEntity & )>;

	  public:
		Scene();
		~Scene();

		Core::ECS::View<Component::Scene::SceneItemComponent> getAllSceneItems() const;

		template<SceneItem T>
		Core::ECS::View<Component::Scene::SceneItemComponent, T> getAllSceneItemsOfType() const
		{
			return ECS_REGISTRY().findComponents<Component::Scene::SceneItemComponent, T>();
		}

		void referenceItem( Component::Scene::SceneItemComponent & p_item );

		const Core::ECS::BaseEntity getItem( const size_t p_index ) const;
		const Core::ECS::BaseEntity getItem( const std::string & p_name ) const;

		template<Core::ECS::ConceptComponent C>
		C & getComponentByIndex( const size_t & p_index ) const
		{
			const Core::ECS::BaseEntity entity = getItem( p_index );
			return ECS_REGISTRY().getComponent<C>( entity );
		}

		template<Core::ECS::ConceptComponent C>
		C & getComponentByName( const std::string & p_name ) const
		{
			const Core::ECS::BaseEntity entity = getItem( p_name );
			return ECS_REGISTRY().getComponent<C>( entity );
		}

		Core::ECS::BaseEntity findItem( const FindItemFunction & p_findFunction ) const;

		bool   isEmpty() const;
		size_t getItemCount() const;

		void update( const float & );

		void clear();
		void reset();

		const Util::Math::AABB & getAABB();

		size_t getItemIndex( const Core::ECS::BaseEntity & p_item ) const;
		void   changeItemIndex( const Core::ECS::BaseEntity & p_item, const size_t p_index );
		void   changeItemsIndex( const std::vector<Core::ECS::BaseEntity> & p_items, const size_t p_position );

		template<Core::ECS::ConceptComponent C>
		size_t getItemIndex( const C & p_item ) const
		{
			const Core::ECS::BaseEntity & entity = ECS_REGISTRY().getEntity( p_item );
			return getItemIndex( entity );
		}
		template<Core::ECS::ConceptComponent C>
		void changeItemIndex( const C & p_item, const size_t p_index )
		{
			const Core::ECS::BaseEntity & entity = ECS_REGISTRY().getEntity( p_item );
			changeItemIndex( entity, p_index );
		}
		template<Core::ECS::ConceptComponent C>
		void changeItemsIndex( const std::vector<const C *> & p_items, const size_t p_position )
		{
			std::vector<Core::ECS::BaseEntity> p_itemEntities = std::vector<Core::ECS::BaseEntity>();
			p_itemEntities.reserve( p_items.size() );
			for ( const Component::Scene::SceneItemComponent * const itemPtr : p_items )
			{
				const Core::ECS::BaseEntity itemEntity = ECS_REGISTRY().getEntity( *itemPtr );
				p_itemEntities.emplace_back( itemEntity );
			}

			changeItemsIndex( p_itemEntities, p_position );
		}

		void sortItemsBySceneIndex( std::vector<Core::ECS::BaseEntity> & p_items ) const;

		template<Core::ECS::ConceptComponent C>
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

		inline const Component::Render::Camera & getCamera() const { return *_camera; }
		inline Component::Render::Camera &		 getCamera() { return *_camera; }

		// Callbacks
		Util::Callback<const Component::Scene::SceneItemComponent &> onSceneItemAdded;

	  private:
		int _persistentIDCounter = 0;

		void _computeAABB();
		void _createDefaultPath();
		void _createDefaultColorLayout();
		void _createDefaultRenderSettings();

	  private:
		Component::Render::Camera *		   _camera		= nullptr;
		Util::Math::AABB				   _aabb		= Util::Math::AABB();
		std::vector<Core::ECS::BaseEntity> _itemIndexes = std::vector<Core::ECS::BaseEntity>();

		void _onSceneItemIsConstruct( Component::Scene::SceneItemComponent & p_sceneItemComponent );

		void _applySceneID( Component::Scene::SceneItemComponent & p_item );
	};
} // namespace VTX::App::Application
#endif
