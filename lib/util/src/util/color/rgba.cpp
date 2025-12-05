#include "util/color/rgba.hpp"
#include "util/math.hpp"

namespace VTX::Util::Color
{
	std::ostream & operator<<( std::ostream & p_os, const Rgba & p_c )
	{
		p_os << p_c.x() << " - " << p_c.y() << " - " << p_c.z() << " - " << p_c.w() << std::endl;
		return p_os;
	}

	void Rgba::saturate()
	{
		_v.x = Util::Math::clamp( _v.x, 0.f, 1.f );
		_v.y = Util::Math::clamp( _v.y, 0.f, 1.f );
		_v.z = Util::Math::clamp( _v.z, 0.f, 1.f );
	}

	void Rgba::applyGamma( const float & pyamma )
	{
		_v.x = powf( _v.x, pyamma );
		_v.y = powf( _v.y, pyamma );
		_v.z = powf( _v.z, pyamma );
	}

	void Rgba::oppose()
	{
		_v.x = 1.f - _v.x;
		_v.y = 1.f - _v.y;
		_v.z = 1.f - _v.z;
	}

	Rgba Rgba::random()
	{
		return Rgba( Util::Math::randomFloat(), Util::Math::randomFloat(), Util::Math::randomFloat() );
	}

} // namespace VTX::Util::Color
