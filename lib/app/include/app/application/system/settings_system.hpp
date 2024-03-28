#ifndef __VTX_APP_APPLICATION_SYSTEM_SETTINGS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_SETTINGS_SYSTEM__

#include "app/application/settings/settings.hpp"
#include "app/application/system/system_registration.hpp"

namespace VTX::App::Application::System
{
	class Settings : public System::AutoRegistrateSystem<Settings>
	{
	  public:
		Application::Settings::Settings currentSetting;
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	// Direct access to the currentSettings
	Application::Settings::Settings & SETTINGS();
} // namespace VTX::App

#endif
