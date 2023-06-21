#ifndef __VTX_APP_APPLICATION_SCENE__
#define __VTX_APP_APPLICATION_SCENE__

#include "app/ecs/component/scene_item_component.hpp"
#include "app/ecs/core/registry.hpp"
#include "app/ecs/registry_manager.hpp"
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
	concept SceneItem = requires( T sceneItem ) { std::derived_from<T, ECS::Component::SceneItemComponent>; };

	class Scene : public Old::Application::Generic::BaseUpdatable
	{
	  public:
		Scene();
		~Scene();

		auto getAllSceneItems() const;

		template<SceneItem T>
		auto getAllSceneItemsOftype() const
		{
			return ECS::MAIN_REGISTRY().getComponents<ECS::Component::SceneItemComponent, T>();
		}

		ECS::Core::BaseEntity createItem();

		const ECS::Core::BaseEntity getItem( const size_t p_index ) const;
		const ECS::Core::BaseEntity getItem( const std::string & p_name ) const;
		bool						isEmpty() const;
		size_t						getItemCount() const;

		virtual void update( const float & ) override;

		void clear();
		void reset();

		const Util::Math::AABB & getAABB();

		int	 getItemIndex( const ECS::Component::SceneItemComponent & p_item ) const;
		void changeItemIndex( const ECS::Component::SceneItemComponent & p_item, const int p_index );
		void changeItemsIndex( const std::vector<const ECS::Component::SceneItemComponent *> & p_items,
							   const int													   p_position );
		void sortItemsBySceneIndex( std::vector<ECS::Component::SceneItemComponent *> & p_items ) const;

	  private:
		int _persistentIDCounter = 0;

		void _computeAABB();
		void _createDefaultPath();

	  private:
		std::unique_ptr<Old::Internal::Scene::CameraManager> _cameraManager;
		Util::Math::AABB									 _aabb;

		void _applySceneID( ECS::Component::SceneItemComponent & p_item );
	};
} // namespace VTX::App::Application
#endif
