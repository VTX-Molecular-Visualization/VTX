#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_CAMERA__
#define __VTX_APP_COMPONENT_RENDER_PROXY_CAMERA__

#include "app/core/renderer/proxy_wrapper.hpp"
#include <memory>
#include <renderer/facade.hpp>
#include <renderer/proxy/camera.hpp>

namespace VTX::App::Component::Render
{
	class ProxyCamera
	{
	  public:
		ProxyCamera();
		~ProxyCamera();

		void setInRenderer( Renderer::Facade & p_renderer );

	  private:
		void _generateProxy();
		void _initCallbacks();

		Core::Renderer::ProxyWrapper<VTX::Renderer::Proxy::Camera> _proxyWrapper;
	};

} // namespace VTX::App::Component::Render
#endif
