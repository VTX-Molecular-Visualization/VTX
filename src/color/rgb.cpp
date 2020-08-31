#include "rgb.hpp"

namespace VTX
{
	namespace Color
	{
		std::ostream & operator<<( std::ostream & p_os, const Rgb & p_c )
		{
			p_os << p_c._r << " - " << p_c._g << " - " << p_c._b << std::endl;
			return p_os;
		}

		const Rgb Rgb::BLACK( 0.f, 0.f, 0.f );
		const Rgb Rgb::WHITE( 1.f, 1.f, 1.f );
		const Rgb Rgb::GREY( 0.5f, 0.5f, 0.5f );
		const Rgb Rgb::RED( 1.f, 0.f, 0.f );
		const Rgb Rgb::GREEN( 0.f, 1.f, 0.f );
		const Rgb Rgb::BLUE( 0.f, 0.f, 1.f );
		const Rgb Rgb::YELLOW( 1.f, 1.f, 0.f );
		const Rgb Rgb::MAGENTA( 1.f, 0.f, 1.f );
		const Rgb Rgb::CYAN( 0.f, 1.f, 1.f );
	} // namespace Color
} // namespace VTX
