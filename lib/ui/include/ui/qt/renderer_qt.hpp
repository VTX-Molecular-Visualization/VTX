#ifndef __VTX_UI_QT_RENDERER__
#define __VTX_UI_QT_RENDERER__

#include <renderer/facade.hpp>

namespace VTX::UI::QT
{
	class RendererQt
	{
	  private:
		inline static int MAKE_CURRENT_COUNT = 0;

	  public:
		RendererQt( Renderer::Facade & p_renderer );
		~RendererQt();

		Renderer::Facade &		 get();
		const Renderer::Facade & get() const;

	  private:
		Renderer::Facade & _rendererFacade;
	};
} // namespace VTX::UI::QT

#endif
