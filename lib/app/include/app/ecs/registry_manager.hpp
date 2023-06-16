#ifndef __VTX_APP_ECS_REGISTRY_MANAGER__
#define __VTX_APP_ECS_REGISTRY_MANAGER__

#include "app/ecs/core/registry.hpp"
#include <util/generic/base_static_singleton.hpp>

namespace VTX::App::ECS
{
	class RegistryManager : public Util::Generic::BaseStaticSingleton<RegistryManager>
	{
	  public:
		RegistryManager( StructPrivacyToken ) {};
		Core::Registry & getRegistry() { return _registry; }

	  private:
		Core::Registry _registry = Core::Registry();
	};

	inline Core::Registry & MAIN_REGISTRY() { return RegistryManager::get().getRegistry(); }
} // namespace VTX::App::ECS
#endif
