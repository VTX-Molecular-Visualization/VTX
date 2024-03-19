#include "app/application/renderer/renderer_accessor.hpp"

namespace VTX::App::Application::Renderer
{
	RendererAccessor::RendererAccessor(
		Renderer &				 p_renderer,
		const Util::Callback<> & p_onGet,
		const Util::Callback<> & p_onRelease
	) :
		_renderer( p_renderer ),
		onGet( p_onGet ), onRelease( p_onRelease )
	{
		onGet();
	}
	RendererAccessor::~RendererAccessor() { onRelease(); }

	Renderer &					  RendererAccessor::get() { return _renderer; }
	const Renderer &			  RendererAccessor::get() const { return _renderer; }
	VTX::Renderer::Facade &		  RendererAccessor::facade() { return _renderer.get(); }
	const VTX::Renderer::Facade & RendererAccessor::facade() const { return _renderer.get(); }
} // namespace VTX::App::Application::Renderer
