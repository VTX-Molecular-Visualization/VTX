#include "core/struct/color_layout.hpp"

namespace VTX::Core::Struct
{
	ColorLayout::ColorLayout() { layout.resize( LAYOUT_SIZE ); }
	ColorLayout::ColorLayout( const std::vector<Util::Color::Rgba> & p_layout )
	{
		assert( p_layout.size() == LAYOUT_SIZE );
		layout = p_layout;
	}

} // namespace VTX::Core::Struct
