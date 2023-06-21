#ifndef __VTX_APP_APPLICATION_SCENE__
#define __VTX_APP_APPLICATION_SCENE__

#include "app/application/registry_manager.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/old/application/generic/base_updatable.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/event/global.hpp"
#include "app/old/internal/scene/camera_manager.hpp"
#include "app/old/manager/event_manager.hpp"
#include <concepts>
#include <string>
#include <util/math/aabb.hpp>

namespace VTX::App::Application
{
	template<typename T>
	concept SceneItem = requires( T sceneItem ) { std::derived_from<T, Component::Scene::SceneItemComponent>; };

	class Scene : public Old::Application::Generic::BaseUpdatable
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

		Core::ECS::BaseEntity createItem();

		const Core::ECS::BaseEntity getItem( const size_t p_index ) const;
		const Core::ECS::BaseEntity getItem( const std::string & p_name ) const;
		bool						isEmpty() const;
		size_t						getItemCount() const;

		virtual void update( const float & ) override;

		void clear();
		void reset();

		const Util::Math::AABB & getAABB();

		int	 getItemIndex( const Component::Scene::SceneItemComponent & p_item ) const;
		void changeItemIndex( const Component::Scene::SceneItemComponent & p_item, const int p_index );
		void changeItemsIndex( const std::vector<const Component::Scene::SceneItemComponent *> & p_items,
							   const int														 p_position );
		void sortItemsBySceneIndex( std::vector<Component::Scene::SceneItemComponent *> & p_items ) const;

	  private:
		int _persistentIDCounter = 0;

		void _computeAABB();
		void _createDefaultPath();

	  private:
		std::unique_ptr<Old::Internal::Scene::CameraManager> _cameraManager = nullptr;
		Util::Math::AABB									 _aabb;

		void _applySceneID( Component::Scene::SceneItemComponent & p_item );
	};
} // namespace VTX::App::Application
#endif
