#ifndef __VTX_APP_CORE_ECS_SYSTEM__
#define __VTX_APP_CORE_ECS_SYSTEM__

#include "registry.hpp"
#include <util/singleton.hpp>

namespace VTX::App::Core::ECS
{
	class ECSSystem
	{
	  public:
		Registry registry;
	};
} // namespace VTX::App::Core::ECS

namespace VTX::App
{
	inline Core::ECS::ECSSystem & ECS_SYSTEM() { return Util::Singleton<Core::ECS::ECSSystem>::get(); }
	inline Core::ECS::Registry &  ECS_REGISTRY() { return ECS_SYSTEM().registry; }
} // namespace VTX::App

#endif
