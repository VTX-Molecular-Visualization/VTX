#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_COLOR_LAYOUT__
#define __VTX_APP_COMPONENT_RENDER_PROXY_COLOR_LAYOUT__

#include "app/application/renderer/proxy_wrapper.hpp"
#include <core/chemdb/color.hpp>
#include <renderer/facade.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Render
{
	class ProxyColorLayout
	{
	  public:
		ProxyColorLayout();
		~ProxyColorLayout();

		void																	 setup( Renderer::Facade & p_renderer );
		Application::Renderer::ProxyWrapper<VTX::Renderer::Proxy::ColorLayout> & getProxy() { return _proxyWrapper; };

	  private:
		void _addInRenderer( Renderer::Facade & p_renderer );
		void _setupCallbacks();

		Application::Renderer::ProxyWrapper<VTX::Renderer::Proxy::ColorLayout> _proxyWrapper;
	};

} // namespace VTX::App::Component::Render
#endif
