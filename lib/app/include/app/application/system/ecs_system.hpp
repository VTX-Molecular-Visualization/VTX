#ifndef __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__

#include "app/application/ecs/component_meta_function.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/system/system_registration.hpp"
#include "app/core/system/base_system.hpp"

namespace VTX::App::Application::System
{
	class ECSSystem : public Core::System::BaseSystem
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
		inline static const SystemRegistration<ECSSystem> SYSTEM = SystemRegistration<ECSSystem>();

		Application::ECS::RegistryManager		registryManager		  = Application::ECS::RegistryManager();
		Application::ECS::ComponentMetaFunction componentMetaFunction = Application::ECS::ComponentMetaFunction();
		Application::ECS::EntityDirector		entityDirector		  = Application::ECS::EntityDirector();
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	Application::ECS::RegistryManager &		  MAIN_REGISTRY();
	Application::ECS::ComponentMetaFunction & COMPONENT_META_FUNCTION();
	Application::ECS::EntityDirector &		  ENTITY_DIRECTOR();
} // namespace VTX::App

#endif
