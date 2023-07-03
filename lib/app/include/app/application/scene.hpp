#ifndef __VTX_APP_APPLICATION_SCENE__
#define __VTX_APP_APPLICATION_SCENE__

#include "app/application/ecs/registry_manager.hpp"
#include "app/component/render/_fwd.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/core/callback_event.hpp"
#include "app/core/ecs/registry.hpp"
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

		auto getAllSceneItems() const;

		template<SceneItem T>
		auto getAllSceneItemsOftype() const
		{
			return MAIN_REGISTRY().getComponents<Component::Scene::SceneItemComponent, T>();
		}

		void referenceItem( Component::Scene::SceneItemComponent & p_item );

		const Core::ECS::BaseEntity getItem( const size_t p_index ) const;
		const Core::ECS::BaseEntity getItem( const std::string & p_name ) const;
		bool						isEmpty() const;
		size_t						getItemCount() const;

		virtual void update( const float & );

		void clear();
		void reset();

		const Util::Math::AABB & getAABB();

		int	 getItemIndex( const Component::Scene::SceneItemComponent & p_item ) const;
		void changeItemIndex( const Component::Scene::SceneItemComponent & p_item, const int p_index );
		void changeItemsIndex( const std::vector<const Component::Scene::SceneItemComponent *> & p_items,
							   const int														 p_position );
		void sortItemsBySceneIndex( std::vector<Component::Scene::SceneItemComponent *> & p_items ) const;

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
		Component::Render::Camera * _camera = nullptr;
		Util::Math::AABB			_aabb	= Util::Math::AABB();

		void _onSceneItemIsConstruct( Component::Scene::SceneItemComponent & p_sceneItemComponent );

		void _applySceneID( Component::Scene::SceneItemComponent & p_item );

		// Callbacks
		Core::CallbackEmitter<Component::Scene::SceneItemComponent &> _onSceneItemAddedCallback
			= Core::CallbackEmitter<Component::Scene::SceneItemComponent &>();
	};
} // namespace VTX::App::Application
#endif
