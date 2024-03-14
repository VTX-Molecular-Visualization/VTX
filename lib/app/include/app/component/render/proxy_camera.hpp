#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_CAMERA__
#define __VTX_APP_COMPONENT_RENDER_PROXY_CAMERA__

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

		std::unique_ptr<VTX::Renderer::Proxy::Camera> _proxyPtr = nullptr;
	};

} // namespace VTX::App::Component::Render
#endif
