#include "ui/qt/renderer_qt.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include <app/vtx_app.hpp>

namespace VTX::UI::QT
{
	RendererQt::RendererQt( Renderer::Facade & p_rendererFacade ) : _rendererFacade( p_rendererFacade )
	{
		if ( MAKE_CURRENT_COUNT == 0 )
			QT_APP()->getMainWindow().getRender()->makeCurrentContext();

		MAKE_CURRENT_COUNT++;
	}
	RendererQt::~RendererQt()
	{
		MAKE_CURRENT_COUNT--;

		if ( MAKE_CURRENT_COUNT == 0 )
			QT_APP()->getMainWindow().getRender()->doneCurrentContext();
	}

	Renderer::Facade &		 RendererQt::get() { return _rendererFacade; }
	const Renderer::Facade & RendererQt::get() const { return _rendererFacade; }

} // namespace VTX::UI::QT
