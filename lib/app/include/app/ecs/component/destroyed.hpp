#ifndef __VTX_APP_ECS_COMPONENT_TRANSFORM__
#define __VTX_APP_ECS_COMPONENT_TRANSFORM__

#include "app/ecs/core/base_component.hpp"

namespace VTX::App::ECS::Component
{
	class Transform : public ECS::Core::BaseComponent
	{
	  public:
		Transform() = default;
	};
} // namespace VTX::App::ECS::Component
#endif
