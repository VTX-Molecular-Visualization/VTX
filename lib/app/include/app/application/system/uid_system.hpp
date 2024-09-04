#ifndef __VTX_APP_APPLICATION_SYSTEM_UID_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_UID_SYSTEM__

#include "app/core/uid/uid_registration.hpp"

namespace VTX::App::Application::System
{
	class UIDSystem final : public Core::UID::UIDRegistration
	{
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	// Access to UIDSystem to get / release UIDs.
	inline Application::System::UIDSystem & UID_SYSTEM()
	{
		return Util::Singleton<Application::System::UIDSystem>::get();
	}
} // namespace VTX::App

#endif
