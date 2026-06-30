#include "util/color/rgba.hpp"
#include "util/math.hpp"
#include <algorithm>
#include <array>
#include <limits>

namespace VTX::Util::Color
{
	namespace
	{
		float _sqDistance( const Rgba & p_a, const Rgba & p_b )
		{
			const float dr = p_a.r() - p_b.r();
			const float dg = p_a.g() - p_b.g();
			const float db = p_a.b() - p_b.b();
			return dr * dr + dg * dg + db * db;
		}

		// IEC 61966-2-1.
		float _srgbToLinear( const float p_channel )
		{
			return p_channel <= 0.04045f ? p_channel / 12.92f
										 : Util::Math::pow( ( p_channel + 0.055f ) / 1.055f, 2.4f );
		}

		// IEC 61966-2-1.
		float _linearToSrgb( const float p_channel )
		{
			return p_channel <= 0.0031308f ? p_channel * 12.92f
										   : 1.055f * Util::Math::pow( p_channel, 1.f / 2.4f ) - 0.055f;
		}
	} // namespace

	std::ostream & operator<<( std::ostream & p_os, const Rgba & p_c )
	{
		p_os << p_c.x() << " - " << p_c.y() << " - " << p_c.z() << " - " << p_c.w() << std::endl;
		return p_os;
	}

	Rgba Rgba::mixedWith( const Rgba & p_other, const float p_factor ) const
	{
		const float factor = Util::Math::clamp( p_factor, 0.f, 1.f );
		return Rgba(
			Util::Math::lerp( r(), p_other.r(), factor ),
			Util::Math::lerp( g(), p_other.g(), factor ),
			Util::Math::lerp( b(), p_other.b(), factor ),
			a()
		);
	}

	Rgba Rgba::toLinear() const
	{
		return Rgba( _srgbToLinear( r() ), _srgbToLinear( g() ), _srgbToLinear( b() ), a() );
	}

	Rgba Rgba::toSRGB() const { return Rgba( _linearToSrgb( r() ), _linearToSrgb( g() ), _linearToSrgb( b() ), a() ); }

	Rgba Rgba::withMinBrightness( const float p_minBrightness ) const
	{
		const float current = brightness();
		if ( current >= p_minBrightness )
		{
			return *this;
		}

		const float factor = ( p_minBrightness - current ) / std::max( 1.f - current, 1e-4f );
		Rgba		result = mixedWith( Rgba( 1.f, 1.f, 1.f, a() ), factor );
		result.saturate();
		return result;
	}

	Rgba Rgba::withSaturation( const float p_factor ) const
	{
		const float grey = brightness();
		Rgba		result(
			   grey + ( r() - grey ) * p_factor, grey + ( g() - grey ) * p_factor, grey + ( b() - grey ) * p_factor, a()
		   );
		result.saturate();
		return result;
	}

	Rgba Rgba::toHighContrast() const
	{
		if ( a() == 0.f )
		{
			return *this;
		}

		Rgba		result	   = withSaturation( 1.35f ).withMinBrightness( 0.42f );
		const float maxChannel = std::max( { result.r(), result.g(), result.b() } );
		if ( maxChannel > 0.f )
		{
			result /= maxChannel;
		}

		result = result.withMinBrightness( 0.5f );
		result.saturate();
		return result;
	}

	Rgba Rgba::toColorblind() const
	{
		if ( a() == 0.f )
		{
			return *this;
		}

		constexpr std::array<Rgba, 8> palette
			= { Rgba( 0, 114, 178 ),  Rgba( 230, 159, 0 ), Rgba( 0, 158, 115 ),	 Rgba( 204, 121, 167 ),
				Rgba( 86, 180, 233 ), Rgba( 213, 94, 0 ),  Rgba( 240, 228, 66 ), Rgba( 153, 153, 153 ) };

		const float brightnessValue = brightness();
		if ( brightnessValue < 0.08f )
		{
			return Rgba( 0.45f, 0.45f, 0.45f, a() );
		}

		if ( std::abs( r() - g() ) < 0.08f && std::abs( g() - b() ) < 0.08f )
		{
			const float target = std::clamp( std::max( brightnessValue, 0.38f ), 0.f, 1.f );
			return Rgba( target, target, target, a() );
		}

		size_t bestIndex	= 0;
		float  bestDistance = std::numeric_limits<float>::max();
		for ( size_t i = 0; i < palette.size(); ++i )
		{
			const float distance = _sqDistance( *this, palette[ i ] );
			if ( distance < bestDistance )
			{
				bestDistance = distance;
				bestIndex	 = i;
			}
		}

		Rgba result = palette[ bestIndex ];
		result		= result.mixedWith( Rgba( 1.f, 1.f, 1.f, a() ), std::max( 0.f, brightnessValue - 0.6f ) * 0.35f );
		result		= result.withMinBrightness( 0.38f );
		result.saturate();
		return Rgba( result.r(), result.g(), result.b(), a() );
	}

	void Rgba::saturate()
	{
		_v.x = Util::Math::clamp( _v.x, 0.f, 1.f );
		_v.y = Util::Math::clamp( _v.y, 0.f, 1.f );
		_v.z = Util::Math::clamp( _v.z, 0.f, 1.f );
	}

	void Rgba::applyGamma( const float & p_gamma )
	{
		_v.x = Util::Math::pow( _v.x, p_gamma );
		_v.y = Util::Math::pow( _v.y, p_gamma );
		_v.z = Util::Math::pow( _v.z, p_gamma );
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
