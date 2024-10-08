#include "app/component/representation/color_layout.hpp"
#include "app/core/renderer/renderer_system.hpp"
#include <core/chemdb/color.hpp>

namespace VTX::App::Component::Representation
{
	ColorLayout::ColorLayout()
	{
		// TODO: where to set default values?
		setColors( VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL.layout );
	}

	void ColorLayout::setupProxy()
	{
		_proxy = std::make_unique<Renderer::Proxy::ColorLayout>( Renderer::Proxy::ColorLayout { &_layout.layout } );
		onChange += _proxy->onChange;

		RENDERER_SYSTEM().onReady() += [ this ]() { RENDERER_SYSTEM().setProxyColorLayout( *_proxy ); };
	}

	void ColorLayout::setColor( const size_t p_index, const Util::Color::Rgba & p_color )
	{
		assert( p_index < VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );

		_layout.layout[ p_index ] = p_color;
		onChange();
	}
	void ColorLayout::setColors( const std::vector<Util::Color::Rgba> & p_colors )
	{
		assert( p_colors.size() == VTX::Core::Struct::ColorLayout::LAYOUT_SIZE );

		_layout.layout = p_colors;
		onChange();
	}

} // namespace VTX::App::Component::Representation
