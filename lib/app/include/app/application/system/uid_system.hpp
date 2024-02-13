#ifndef __VTX_APP_APPLICATION_SYSTEM_UID_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_UID_SYSTEM__

#include "app/application/system/system_registration.hpp"
#include "app/core/system/base_system.hpp"
#include "app/core/uid/uid_registration.hpp"

namespace VTX::App::Application::System
{
	class UIDSystem : public Core::System::BaseSystem, public Core::UID::UIDRegistration
	{
	  public:
		inline static const SystemRegistration<UIDSystem> SYSTEM = SystemRegistration<UIDSystem>();
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	Application::System::UIDSystem & UID_SYSTEM();
}

#endif
