#include "app/application/system/renderer.hpp"

namespace VTX::App::Application::System
{
	Application::Renderer::RendererAccessor Renderer::accessor()
	{
		return Application::Renderer::RendererAccessor( _renderer, onGet, onRelease );
	}

	VTX::Renderer::Facade & Renderer::facade() { return _renderer.get(); }

} // namespace VTX::App::Application::System

namespace VTX::App
{
	Application::System::Renderer &			RENDERER_SYSTEM() { return Application::System::Renderer::SYSTEM.get(); }
	Application::Renderer::RendererAccessor RENDERER() { return RENDERER_SYSTEM().accessor(); }
} // namespace VTX::App
