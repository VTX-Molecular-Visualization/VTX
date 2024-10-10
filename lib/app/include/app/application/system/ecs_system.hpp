#ifndef __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__

#include "app/application/ecs/component_meta_function.hpp"
#include "app/application/ecs/registry_manager.hpp"

namespace VTX::App::Application::System
{
	// ECS System handle 3 sub-systems in order to manage ECS in VTX
	class ECSSystem
	{
	  public:
		template<typename C>
		struct ComponentStaticIDRegistration
		{
			ComponentStaticIDRegistration( const ECS::ComponentStaticID & p_id )
			{
				Util::Singleton<Application::System::ECSSystem>::get()
					.registryManager.getComponentInfo()
					.registerComponent<C>( p_id );
				Util::Singleton<Application::System::ECSSystem>::get().componentMetaFunction.registerComponent<C>( p_id
				);
			}
		};

	  public:
		Application::ECS::RegistryManager		registryManager		  = Application::ECS::RegistryManager();
		Application::ECS::ComponentMetaFunction componentMetaFunction = Application::ECS::ComponentMetaFunction();
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	inline Application::System::ECSSystem & ECS() { return Util::Singleton<Application::System::ECSSystem>::get(); }

	// MAIN_REGISTRY give access to the registry to create / get / delete entities and components
	inline Application::ECS::RegistryManager & MAIN_REGISTRY() { return ECS().registryManager; }

	// COMPONENT_META_FUNCTION give access to the meta functions on Components (i.e. Serialization)
	inline Application::ECS::ComponentMetaFunction & COMPONENT_META_FUNCTION() { return ECS().componentMetaFunction; }
} // namespace VTX::App

#endif
