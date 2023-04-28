#include "util/color/rgba.hpp"

namespace VTX::Util::Color
{
	std::ostream & operator<<( std::ostream & p_os, const Rgba & p_c )
	{
		p_os << p_c.x << " - " << p_c.y << " - " << p_c.z << " - " << p_c.w << std::endl;
		return p_os;
	}

	const Rgba Rgba::BLACK( 0.f, 0.f, 0.f );
	const Rgba Rgba::WHITE( 1.f, 1.f, 1.f );
	const Rgba Rgba::GREY( 0.5f, 0.5f, 0.5f );
	const Rgba Rgba::RED( 1.f, 0.f, 0.f );
	const Rgba Rgba::GREEN( 0.f, 1.f, 0.f );
	const Rgba Rgba::BLUE( 0.f, 0.f, 1.f );
	const Rgba Rgba::YELLOW( 1.f, 1.f, 0.f );
	const Rgba Rgba::MAGENTA( 1.f, 0.f, 1.f );
	const Rgba Rgba::CYAN( 0.f, 1.f, 1.f );
} // namespace VTX::Util::Color
