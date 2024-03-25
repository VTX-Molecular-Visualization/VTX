#include "core/chemdb/color.hpp"

namespace VTX::Core::ChemDB::Color
{
	ColorLayout::ColorLayout() { layout.resize( LAYOUT_SIZE ); }
	ColorLayout::ColorLayout( const std::vector<Util::Color::Rgba> & p_layout )
	{
		assert( p_layout.size() == LAYOUT_SIZE );
		layout = p_layout;
	}

} // namespace VTX::Core::ChemDB::Color
