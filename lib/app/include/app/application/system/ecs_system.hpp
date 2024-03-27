#ifndef __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__

#include "app/application/ecs/component_meta_function.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/system/system_registration.hpp"

namespace VTX::App::Application::System
{
	// ECS System handle 3 sub-systems in order to manage ECS in VTX
	class ECSSystem : public System::AutoRegistrateSystem<ECSSystem>
	{
	  public:
		template<typename C>
		struct ComponentStaticIDRegistration
		{
			ComponentStaticIDRegistration( const ECS::ComponentStaticID & p_id )
			{
				SYSTEM.get().registryManager.getComponentInfo().registerComponent<C>( p_id );
				SYSTEM.get().componentMetaFunction.registerComponent<C>( p_id );
			}
		};

	  public:
		Application::ECS::RegistryManager		registryManager		  = Application::ECS::RegistryManager();
		Application::ECS::ComponentMetaFunction componentMetaFunction = Application::ECS::ComponentMetaFunction();
		Application::ECS::EntityDirector		entityDirector		  = Application::ECS::EntityDirector();
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	// MAIN_REGISTRY give access to the registry to create / get / delete entities and components
	Application::ECS::RegistryManager & MAIN_REGISTRY();

	// COMPONENT_META_FUNCTION give access to the meta functions on Components (i.e. Serialization)
	Application::ECS::ComponentMetaFunction & COMPONENT_META_FUNCTION();

	// ENTITY_DIRECTOR Allows to instantiate ready to used object(an entity with all its necessary components linked on
	// it and initialized.) It can also be used to customize object initialization.
	Application::ECS::EntityDirector & ENTITY_DIRECTOR();
} // namespace VTX::App

#endif
