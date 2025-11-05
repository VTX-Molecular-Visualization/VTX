#include "app/library/preset/color_layout.hpp"

namespace VTX::App::Library::Preset
{
	ColorLayout::ColorLayout()
	{
		// Set all white by default.
		_data.colors.fill( Util::Color::Rgba( 1.f, 1.f, 1.f, 1.f ) );
	}

	void ColorLayout::save() {}
	void ColorLayout::load() {}

	void ColorLayout::setColor( const Index p_index, const Util::Color::Rgba & p_color )
	{
		assert( p_index >= 0 );
		assert( p_index < Renderer::Color::COLOR_LAYOUT_SIZE );

		_data.colors[ p_index ] = p_color;
		onChange( p_index );
	}
	void ColorLayout::setColors( const Renderer::Color::LayoutArray & p_colors )
	{
		assert( p_colors.size() == Renderer::Color::COLOR_LAYOUT_SIZE );

		_data.colors = p_colors;
		onChangeAll();
	}

	const Util::Color::Rgba & ColorLayout::getChainColor( const Index p_index ) const
	{
		assert( p_index >= 0 );
		assert( p_index < Renderer::Color::LAYOUT_COUNT_CHAINS );

		const Index index = p_index % Renderer::Color::LAYOUT_COUNT_CHAINS;

		return _data.colors[ Renderer::Color::LAYOUT_OFFSET_CHAINS + index ];
	}

	const Util::Color::Rgba & ColorLayout::getResidueColor( const Index p_index ) const
	{
		assert( p_index >= 0 );
		assert( p_index < Renderer::Color::LAYOUT_COUNT_RESIDUES );

		const Index index = p_index % Renderer::Color::LAYOUT_COUNT_RESIDUES;

		return _data.colors[ Renderer::Color::LAYOUT_OFFSET_RESIDUES + index ];
	}
} // namespace VTX::App::Library::Preset
