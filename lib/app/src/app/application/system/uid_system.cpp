#include "app/application/system/uid_system.hpp"

namespace VTX::App
{
	Application::System::UIDSystem & UID_SYSTEM() { return Application::System::UIDSystem::SYSTEM.get(); }
} // namespace VTX::App
