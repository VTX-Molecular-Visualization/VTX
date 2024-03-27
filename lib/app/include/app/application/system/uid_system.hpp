#ifndef __VTX_APP_APPLICATION_SYSTEM_UID_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_UID_SYSTEM__

#include "app/application/system/system_registration.hpp"
#include "app/core/uid/uid_registration.hpp"

namespace VTX::App::Application::System
{
	class UIDSystem final : public System::AutoRegistrateSystem<UIDSystem>, public Core::UID::UIDRegistration
	{
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	Application::System::UIDSystem & UID_SYSTEM();
}

#endif
