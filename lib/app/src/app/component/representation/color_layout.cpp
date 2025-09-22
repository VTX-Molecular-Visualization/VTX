#include "app/component/representation/color_layout.hpp"
#include "app/core/renderer/renderer_system.hpp"

namespace VTX::App::Component::Representation
{
	ColorLayout::ColorLayout( Library::Preset::ColorLayout & p_preset ) : BaseComponentProxyPreset( p_preset ) {}

	void ColorLayout::setupProxy()
	{
		_proxy = std::make_unique<Renderer::Proxy::ColorLayout>( _preset.getData() );

		_preset.onChange += [ this ]( const Index p_index ) { _proxy->onChange( p_index ); };
		_preset.onChangeAll += [ this ]() { _proxy->onChangeAll(); };

		RENDERER_SYSTEM().onReady( [ this ]() { RENDERER_SYSTEM().setProxyColorLayout( *_proxy ); } );
	}

} // namespace VTX::App::Component::Representation
