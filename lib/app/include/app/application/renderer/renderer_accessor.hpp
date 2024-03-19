#ifndef __VTX_APP_APPLICATION_RENDERER_RENDERER_ACCESSOR__
#define __VTX_APP_APPLICATION_RENDERER_RENDERER_ACCESSOR__

#include "app/application/renderer/renderer.hpp"
#include <renderer/facade.hpp>
#include <util/callback.hpp>

namespace VTX::App::Application::Renderer
{
	class RendererAccessor
	{
	  public:
		RendererAccessor(
			Renderer &				 p_renderer,
			const Util::Callback<> & p_onGet,
			const Util::Callback<> & p_onRelease
		);
		~RendererAccessor();

		Renderer &		 get();
		const Renderer & get() const;

		VTX::Renderer::Facade &		  facade();
		const VTX::Renderer::Facade & facade() const;

	  private:
		Renderer & _renderer;

		const Util::Callback<> & onGet;
		const Util::Callback<> & onRelease;
	};

} // namespace VTX::App::Application::Renderer
#endif
