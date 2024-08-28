#include "app/application/system/renderer.hpp"

namespace VTX::App
{
	Application::System::Renderer & RENDERER() { return Application::System::Renderer::SYSTEM.get(); }
} // namespace VTX::App
