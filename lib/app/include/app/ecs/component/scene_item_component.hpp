#ifndef __VTX_APP_ECS_COMPONENT_SCENE_ITEM__
#define __VTX_APP_ECS_COMPONENT_SCENE_ITEM__

#include "app/application/scene_utility.hpp"
#include "app/ecs/core/base_component.hpp"
#include <string>

namespace VTX::App::ECS::Component
{
	class SceneItemComponent : public ECS::Core::BaseComponent
	{
	  public:
		virtual ~SceneItemComponent() = default;

		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name ) { _name = p_name; };

		inline int	getPersistentSceneID() const { return _persistentId; }
		inline void setPersistentSceneID( const int p_id ) { _persistentId = p_id; }
		inline bool hasPersistentSceneID() const { return _persistentId != -1; }

		void addUpdateFunction( const std::string & p_key, const SceneUtility::UpdateCallback & p_callback );
		void removeUpdateFunction( const std::string & p_key );

	  private:
		int			_persistentId = -1;
		std::string _name;
	};
} // namespace VTX::App::ECS::Component
#endif
