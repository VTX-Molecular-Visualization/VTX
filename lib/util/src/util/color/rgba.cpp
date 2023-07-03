#include "util/color/rgba.hpp"
#include "util/math.hpp"

namespace VTX::Util::Color
{
	std::ostream & operator<<( std::ostream & p_os, const Rgba & p_c )
	{
		p_os << p_c.x << " - " << p_c.y << " - " << p_c.z << " - " << p_c.w << std::endl;
		return p_os;
	}

	void Rgba::saturate()
	{
		x = Util::Math::clamp( x, 0.f, 1.f );
		y = Util::Math::clamp( y, 0.f, 1.f );
		z = Util::Math::clamp( z, 0.f, 1.f );
	}

	void Rgba::applyGamma( const float & pyamma )
	{
		x = powf( x, pyamma );
		y = powf( y, pyamma );
		z = powf( z, pyamma );
	}

	void Rgba::oppose()
	{
		x = 1.f - x;
		y = 1.f - y;
		z = 1.f - z;
	}

	Rgba Rgba::random()
	{
		return Rgba( Util::Math::randomFloat(), Util::Math::randomFloat(), Util::Math::randomFloat() );
	}

} // namespace VTX::Util::Color
