#ifndef __VTX_UI_QT_RENDERER__
#define __VTX_UI_QT_RENDERER__

#include <renderer/renderer.hpp>

namespace VTX::UI::QT
{
	class RendererQt
	{
	  private:
		inline static int MAKE_CURRENT_COUNT = 0;

	  public:
		RendererQt( Renderer::Renderer & p_renderer );
		~RendererQt();

		Renderer::Renderer &	   get();
		const Renderer::Renderer & get() const;

	  private:
		Renderer::Renderer & _renderer;
	};
} // namespace VTX::UI::QT

#endif
