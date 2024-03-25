#ifndef __VTX_APP_APPLICATION_SYSTEM_SETTINGS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_SETTINGS_SYSTEM__

#include "app/application/settings/settings.hpp"
#include "app/application/system/system_registration.hpp"
#include "app/core/system/base_system.hpp"

namespace VTX::App::Application::System
{
	class Settings : public Core::System::BaseSystem
	{
	  public:
		inline static const System::SystemRegistration<Settings> SYSTEM = System::SystemRegistration<Settings>();

	  public:
		Application::Settings::Settings currentSetting;
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	Application::Settings::Settings & SETTINGS();
}

#endif
