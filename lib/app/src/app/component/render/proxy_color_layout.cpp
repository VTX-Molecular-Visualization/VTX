#include "app/component/render/proxy_color_layout.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/representation/color_layout.hpp"

namespace VTX::App::Component::Render
{
	ProxyColorLayout::ProxyColorLayout() {}
	ProxyColorLayout::~ProxyColorLayout() {}
	void ProxyColorLayout::setup( Renderer::Facade & p_renderer )
	{
		_addInRenderer( p_renderer );
		_setupCallbacks();
	}

	void ProxyColorLayout::_addInRenderer( Renderer::Facade & p_renderer )
	{
		Component::Representation::ColorLayout & colorLayoutComp
			= MAIN_REGISTRY().getComponent<Component::Representation::ColorLayout>( *this );

		colorLayoutComp.setColors( VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL.layout );

		std::unique_ptr<VTX::Renderer::Proxy::ColorLayout> proxyPtr
			= std::make_unique<VTX::Renderer::Proxy::ColorLayout>( VTX::Renderer::Proxy::ColorLayout {
				&colorLayoutComp.getLayout().layout } );

		_proxyWrapper.setProxy( proxyPtr );
	}
	void ProxyColorLayout::_setupCallbacks() {}

} // namespace VTX::App::Component::Render
