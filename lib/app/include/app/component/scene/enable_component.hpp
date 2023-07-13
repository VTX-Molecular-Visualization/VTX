#ifndef __VTX_APP_COMPONENT_SCENE_ENABLE__
#define __VTX_APP_COMPONENT_SCENE_ENABLE__

#include "app/ecs/core/base_component.hpp"

namespace VTX::App::Component::Scene
{
	class EnableComponent : public ECS::Core::BaseComponent
	{
	  public:
		EnableComponent() = default;
		EnableComponent( const bool p_enabled ) { _enabled = p_enabled; };
		virtual ~EnableComponent() = default;

		bool isEnabled() const { return _enabled; }
		bool setEnabled( bool p_enabled ) { _enabled = p_enabled; }

	  private:
		bool _enabled = true;
	};
} // namespace VTX::App::Component::Scene
#endif