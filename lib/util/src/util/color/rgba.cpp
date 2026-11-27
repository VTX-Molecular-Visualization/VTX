#include "util/color/rgba.hpp"
#include "util/math.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <fmt/format.h>
#include <limits>
#include <stdexcept>

namespace VTX::Util::Color
{
	namespace
	{
		float _sqDistance( const Rgba & p_a, const Rgba & p_b ) noexcept
		{
			const float dr = p_a.r() - p_b.r();
			const float dg = p_a.g() - p_b.g();
			const float db = p_a.b() - p_b.b();
			return dr * dr + dg * dg + db * db;
		}

		// IEC 61966-2-1.
		float _srgbToLinear( const float p_channel ) noexcept
		{
			return p_channel <= 0.04045f ? p_channel / 12.92f
										 : Util::Math::pow( ( p_channel + 0.055f ) / 1.055f, 2.4f );
		}

		// IEC 61966-2-1.
		float _linearToSrgb( const float p_channel ) noexcept
		{
			return p_channel <= 0.0031308f ? p_channel * 12.92f
										   : 1.055f * Util::Math::pow( p_channel, 1.f / 2.4f ) - 0.055f;
		}
	} // namespace

	Rgba Rgba::mixedWith( const Rgba & p_other, const float p_factor ) const noexcept
	{
		const float factor = Util::Math::clamp( p_factor, 0.f, 1.f );
		return Rgba(
			Util::Math::lerp( r(), p_other.r(), factor ),
			Util::Math::lerp( g(), p_other.g(), factor ),
			Util::Math::lerp( b(), p_other.b(), factor ),
			a()
		);
	}

	Rgba Rgba::toLinear() const noexcept
	{ return Rgba( _srgbToLinear( r() ), _srgbToLinear( g() ), _srgbToLinear( b() ), a() ); }

	Rgba Rgba::toSRGB() const noexcept
	{ return Rgba( _linearToSrgb( r() ), _linearToSrgb( g() ), _linearToSrgb( b() ), a() ); }

	Rgba Rgba::withMinBrightness( const float p_minBrightness ) const noexcept
	{
		const float current = brightness();
		if ( current >= p_minBrightness )
		{
			return *this;
		}

		const float factor = ( p_minBrightness - current ) / Util::Math::max( 1.f - current, 1e-4f );
		Rgba		result = mixedWith( Rgba( 1.f, 1.f, 1.f, a() ), factor );
		result.saturate();
		return result;
	}

	Rgba Rgba::withSaturation( const float p_factor ) const noexcept
	{
		const float grey = brightness();
		Rgba		result(
			grey + ( r() - grey ) * p_factor, grey + ( g() - grey ) * p_factor, grey + ( b() - grey ) * p_factor, a()
		);
		result.saturate();
		return result;
	}

	Rgba Rgba::toHighContrast() const noexcept
	{
		if ( a() == 0.f )
		{
			return *this;
		}

		Rgba		result	   = withSaturation( 1.35f ).withMinBrightness( 0.42f );
		const float maxChannel = Util::Math::max( result.r(), Util::Math::max( result.g(), result.b() ) );
		if ( maxChannel > 0.f )
		{
			result /= maxChannel;
		}

		result = result.withMinBrightness( 0.5f );
		result.saturate();
		return Rgba( result.r(), result.g(), result.b(), a() );
	}

	Rgba Rgba::toColorblind() const noexcept
	{
		if ( a() == 0.f )
		{
			return *this;
		}

		constexpr auto palette = std::to_array<Rgba>( { Rgba( 0, 114, 178 ),
														Rgba( 230, 159, 0 ),
														Rgba( 0, 158, 115 ),
														Rgba( 204, 121, 167 ),
														Rgba( 86, 180, 233 ),
														Rgba( 213, 94, 0 ),
														Rgba( 240, 228, 66 ),
														Rgba( 153, 153, 153 ) } );

		const float brightnessValue = brightness();
		if ( brightnessValue < 0.08f )
		{
			return Rgba( 0.45f, 0.45f, 0.45f, a() );
		}

		if ( Util::Math::abs( r() - g() ) < 0.08f && Util::Math::abs( g() - b() ) < 0.08f )
		{
			const float target = Util::Math::clamp( Util::Math::max( brightnessValue, 0.38f ), 0.f, 1.f );
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
		result = result.mixedWith( Rgba( 1.f, 1.f, 1.f, a() ), Util::Math::max( 0.f, brightnessValue - 0.6f ) * 0.35f );
		result = result.withMinBrightness( 0.38f );
		result.saturate();
		return Rgba( result.r(), result.g(), result.b(), a() );
	}

	void Rgba::saturate() noexcept
	{
		_v.x = Util::Math::clamp( _v.x, 0.f, 1.f );
		_v.y = Util::Math::clamp( _v.y, 0.f, 1.f );
		_v.z = Util::Math::clamp( _v.z, 0.f, 1.f );
		_v.w = Util::Math::clamp( _v.w, 0.f, 1.f );
	}

	void Rgba::applyGamma( const float & p_gamma ) noexcept
	{
		_v.x = Util::Math::pow( _v.x, p_gamma );
		_v.y = Util::Math::pow( _v.y, p_gamma );
		_v.z = Util::Math::pow( _v.z, p_gamma );
	}

	void Rgba::oppose() noexcept
	{
		_v.x = 1.f - _v.x;
		_v.y = 1.f - _v.y;
		_v.z = 1.f - _v.z;
	}

	namespace
	{
		uint _toPacked( const Vec4f & p_color ) noexcept
		{
			const auto toByte = []( const float p_channel ) -> uint
			{ return static_cast<uint>( std::lround( std::clamp( p_channel, 0.f, 1.f ) * 255.f ) ); };

			return toByte( p_color.x ) << 24 | toByte( p_color.y ) << 16 | toByte( p_color.z ) << 8
				   | toByte( p_color.w );
		}
	} // namespace

	std::string Rgba::toHexaString() const { return fmt::format( "#{:08X}", _toPacked( _v ) ); }

	std::string Rgba::toHexaString( const Rgba & p_color ) { return p_color.toHexaString(); }

	void Rgba::fromHexaString( const std::string_view p_hexa )
	{
		const bool hasAlpha = p_hexa.size() == 9;
		if ( ( p_hexa.size() == 7 || hasAlpha ) && p_hexa.front() == '#' )
		{
			const char * const end	  = p_hexa.data() + p_hexa.size();
			uint			   packed = 0;

			if ( std::from_chars( p_hexa.data() + 1, end, packed, 16 ).ptr == end )
			{
				if ( not hasAlpha )
				{
					packed = packed << 8 | 0xFF;
				}

				*this = Rgba(
					static_cast<int>( packed >> 24 ),
					static_cast<int>( packed >> 16 & 0xFF ),
					static_cast<int>( packed >> 8 & 0xFF ),
					static_cast<int>( packed & 0xFF )
				);
				return;
			}
		}

		throw std::invalid_argument( "Invalid color \"" + std::string( p_hexa ) + "\", expected #RRGGBB or #RRGGBBAA" );
	}

	void Rgba::fromHexaString( const std::string_view p_hexa, Rgba & p_color ) { p_color.fromHexaString( p_hexa ); }

	Rgba Rgba::random()
	{ return Rgba( Util::Math::randomFloat(), Util::Math::randomFloat(), Util::Math::randomFloat() ); }

} // namespace VTX::Util::Color
