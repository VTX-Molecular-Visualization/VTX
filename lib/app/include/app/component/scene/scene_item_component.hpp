#ifndef __VTX_APP_COMPONENT_SCENE_SCENE_ITEM__
#define __VTX_APP_COMPONENT_SCENE_SCENE_ITEM__

#include "app/application/scene_utility.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/core/ecs/base_component.hpp"
#include <string>
#include <util/callback.hpp>

namespace VTX::App::Component::Scene
{
	class SceneItemComponent : public Core::ECS::BaseComponent
	{
	  public:
		virtual ~SceneItemComponent() = default;

		const std::string & getName() const { return _name; };
		void				setName( const std::string & p_name )
		{
			if ( _name != p_name )
			{
				_name = p_name;
				onName( _name );
			}
		};

		inline int	getPersistentSceneID() const { return _persistentId; }
		inline void setPersistentSceneID( const int p_id ) { _persistentId = p_id; }
		inline bool hasPersistentSceneID() const { return _persistentId != -1; }

		// Callbacks
		Util::Callback<std::string_view> onName;

	  private:
		int			_persistentId = -1;
		std::string _name		  = "";
	};
} // namespace VTX::App::Component::Scene
#endif
