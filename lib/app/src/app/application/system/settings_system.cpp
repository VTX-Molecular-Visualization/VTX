#include "app/application/system/settings_system.hpp"

namespace VTX::App
{
	Application::Settings::Settings & SETTINGS() { return Application::System::Settings::SYSTEM.get().currentSetting; }
} // namespace VTX::App
