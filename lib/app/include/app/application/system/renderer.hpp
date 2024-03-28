#ifndef __VTX_APP_APPLICATION_SYSTEM_RENDERER__
#define __VTX_APP_APPLICATION_SYSTEM_RENDERER__

#include "app/application/renderer/renderer.hpp"
#include "app/application/renderer/renderer_accessor.hpp"
#include "app/application/system/system_registration.hpp"
#include <renderer/facade.hpp>
#include <util/callback.hpp>

namespace VTX::App::Application::System
{
	class Renderer : public System::AutoRegistrateSystem<Renderer>
	{
	  public:
		Renderer() = default;
		Application::Renderer::RendererAccessor accessor();
		VTX::Renderer::Facade &					facade();

		Util::Callback<> onGet;
		Util::Callback<> onRelease;

	  private:
		Application::Renderer::Renderer _renderer;
	};

} // namespace VTX::App::Application::System

namespace VTX::App
{
	// RENDERER_SYSTEM give an access the System::Renderer object
	Application::System::Renderer & RENDERER_SYSTEM();

	// RENDERER return an accessor to the Renderer::Facade object with onGet/onRelease functions called (to activate
	// openGLContext for example)
	Application::Renderer::RendererAccessor RENDERER();
} // namespace VTX::App
#endif
