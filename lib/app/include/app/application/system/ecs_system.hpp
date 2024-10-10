#ifndef __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__

#include "app/core/ecs/registry.hpp"
#include <util/singleton.hpp>

namespace VTX::App::Application::System
{
	class ECSSystem
	{
	  public:
		Core::ECS::Registry registry;
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	inline Application::System::ECSSystem & ECS() { return Util::Singleton<Application::System::ECSSystem>::get(); }
	inline Core::ECS::Registry &			ECS_REGISTRY() { return ECS().registry; }
} // namespace VTX::App

#endif
