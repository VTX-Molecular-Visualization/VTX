#include "app/library/preset/color_layout.hpp"
#include <core/chemdb/color_layout.hpp>

namespace VTX::App::Library::Preset
{
	ColorLayout::ColorLayout()
	{
		// setColorLayouts( VTX::Core::ChemDB::ColorLayout::COLOR_LAYOUT_JMOL.layout );
	}

	void ColorLayout::save() {}
	void ColorLayout::load() {}

	void ColorLayout::setColor( const Index p_index, const Util::Color::Rgba & p_color )
	{
		assert( p_index >= 0 );
		assert( p_index < VTX::Core::Struct::COLOR_LAYOUT_SIZE );

		_data.layout[ p_index ] = p_color;
		onChange( p_index );
	}
	void ColorLayout::setColors( const VTX::Core::Struct::ColorLayoutArray & p_colors )
	{
		assert( p_colors.size() == VTX::Core::Struct::COLOR_LAYOUT_SIZE );

		_data.layout = p_colors;
		onChangeAll();
	}

	const Util::Color::Rgba & ColorLayout::getChainColor( const Index p_index ) const
	{
		assert( p_index >= 0 );
		assert( p_index < VTX::Core::ChemDB::ColorLayout::LAYOUT_COUNT_CHAINS );

		const Index index = p_index % VTX::Core::ChemDB::ColorLayout::LAYOUT_COUNT_CHAINS;

		return _data.layout[ VTX::Core::ChemDB::ColorLayout::LAYOUT_OFFSET_CHAINS + index ];
	}

	const Util::Color::Rgba & ColorLayout::getResidueColor( const Index p_index ) const
	{
		assert( p_index >= 0 );
		assert( p_index < VTX::Core::ChemDB::ColorLayout::LAYOUT_COUNT_RESIDUES );

		const Index index = p_index % VTX::Core::ChemDB::ColorLayout::LAYOUT_COUNT_RESIDUES;

		return _data.layout[ VTX::Core::ChemDB::ColorLayout::LAYOUT_OFFSET_RESIDUES + index ];
	}
} // namespace VTX::App::Library::Preset
