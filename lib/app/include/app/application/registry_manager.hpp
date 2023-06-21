#ifndef __VTX_APP_APPLICATION_ECS_REGISTRY_MANAGER__
#define __VTX_APP_APPLICATION_ECS_REGISTRY_MANAGER__

#include "app/core/ecs/registry.hpp"
#include <util/generic/base_static_singleton.hpp>

namespace VTX::App::Application
{
	class RegistryManager : public Util::Generic::BaseStaticSingleton<RegistryManager>
	{
	  public:
		RegistryManager( StructPrivacyToken ) {};
		Core::ECS::Registry & getRegistry() { return _registry; }

	  private:
		Core::ECS::Registry _registry = Core::ECS::Registry();
	};

	inline Core::ECS::Registry & MAIN_REGISTRY() { return RegistryManager::get().getRegistry(); }
} // namespace VTX::App::Application
#endif
