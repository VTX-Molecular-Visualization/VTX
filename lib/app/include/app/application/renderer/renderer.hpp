#ifndef __VTX_APP_APPLICATION_RENDERER_RENDERER__
#define __VTX_APP_APPLICATION_RENDERER_RENDERER__

#include <memory>
#include <renderer/facade.hpp>

namespace VTX::App::Application::Renderer
{
	class Renderer
	{
	  public:
		Renderer() = default;
		void init();

		VTX::Renderer::Facade &		  get();
		const VTX::Renderer::Facade & get() const;

	  private:
		std::unique_ptr<VTX::Renderer::Facade> _rendererPtr = nullptr;
	};

} // namespace VTX::App::Application::Renderer
#endif
