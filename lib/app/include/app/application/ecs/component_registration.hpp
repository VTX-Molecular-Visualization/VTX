#ifndef __VTX_APP_APPLICATION_ECS_COMPONENT_REGISTRATION__
#define __VTX_APP_APPLICATION_ECS_COMPONENT_REGISTRATION__

#include "app/application/ecs/component_info.hpp"
#include "app/application/ecs/component_meta_function.hpp"

namespace VTX::App::Application::ECS
{
	template<typename C>
	struct Registration
	{
		Registration( const ComponentID & p_id )
		{
			ComponentInfo::get().registerComponent<C>( p_id );
			ComponentMetaFunction::get().registerComponent<C>( p_id );
		}
	};
} // namespace VTX::App::Application::ECS
#endif
