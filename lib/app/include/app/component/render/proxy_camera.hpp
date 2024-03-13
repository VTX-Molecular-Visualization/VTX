#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_CAMERA__
#define __VTX_APP_COMPONENT_RENDER_PROXY_CAMERA__

#include "app/component/render/camera.hpp"
#include <memory>
#include <renderer/proxy/camera.hpp>

namespace VTX::App::Component::Render
{
	class ProxyCamera
	{
	  public:
		ProxyCamera();
		void init();

		VTX::Renderer::Proxy::Camera &		 getProxy() { return *_proxyPtr; }
		const VTX::Renderer::Proxy::Camera & getProxy() const { return *_proxyPtr; }

	  private:
		void _generateProxy();
		void _initCallbacks();

		std::unique_ptr<VTX::Renderer::Proxy::Camera> _proxyPtr = nullptr;
	};

} // namespace VTX::App::Component::Render
#endif
