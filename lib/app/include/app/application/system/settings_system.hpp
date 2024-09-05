#ifndef __VTX_APP_APPLICATION_SYSTEM_SETTINGS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_SETTINGS_SYSTEM__

#include "app/application/settings/settings.hpp"

namespace VTX::App::Application::System
{
	class Settings
	{
	  public:
		Application::Settings::Settings currentSetting;
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	// Direct access to the currentSettings
	inline Application::Settings::Settings & SETTINGS()
	{
		return Util::Singleton<Application::System::Settings>::get().currentSetting;
	}
} // namespace VTX::App

#endif
