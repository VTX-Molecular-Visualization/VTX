#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_COLOR_LAYOUT__
#define __VTX_APP_COMPONENT_RENDER_PROXY_COLOR_LAYOUT__

#include "app/core/renderer/proxy_wrapper.hpp"
#include <renderer/facade.hpp>
#include <renderer/proxy/color_layout.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Render
{
	class ProxyColorLayout
	{
	  public:
		ProxyColorLayout();
		~ProxyColorLayout();

		void															  setup( Renderer::Facade & p_renderer );
		Core::Renderer::ProxyWrapper<VTX::Renderer::Proxy::ColorLayout> & getProxy() { return _proxyWrapper; };

	  private:
		void _addInRenderer( Renderer::Facade & p_renderer );
		void _setupCallbacks();

		Core::Renderer::ProxyWrapper<VTX::Renderer::Proxy::ColorLayout> _proxyWrapper;
	};

} // namespace VTX::App::Component::Render
#endif
