#pragma once

#include "util/types.hpp"
#include <cassert>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace VTX::Util::Color
{
	/**
	 * @brief Color RGBA (glm::vec4 wrapper).
	 */
	class Rgba
	{
	  public:
		static constexpr float BYTE_TO_FLOAT = 1.f / 255.f;

		constexpr Rgba() = default;

		constexpr Rgba( float p_r, float p_g, float p_b, float p_a ) noexcept : _v( p_r, p_g, p_b, p_a ) {}

		constexpr Rgba( float p_r, float p_g, float p_b ) noexcept : _v( p_r, p_g, p_b, 1.f ) {}

		constexpr Rgba( int p_r, int p_g, int p_b, int p_a ) noexcept :
			_v( p_r * BYTE_TO_FLOAT, p_g * BYTE_TO_FLOAT, p_b * BYTE_TO_FLOAT, p_a * BYTE_TO_FLOAT )
		{
		}

		constexpr Rgba( int p_r, int p_g, int p_b ) noexcept :
			_v( p_r * BYTE_TO_FLOAT, p_g * BYTE_TO_FLOAT, p_b * BYTE_TO_FLOAT, 1.f )
		{
		}

		explicit Rgba( const std::vector<float> & p_c ) noexcept
		{
			assert( p_c.size() == 4 );
			_v.x = p_c[ 0 ];
			_v.y = p_c[ 1 ];
			_v.z = p_c[ 2 ];
			_v.w = p_c[ 3 ];
		}

		constexpr explicit Rgba( const Vec4f & p_v ) noexcept : _v( p_v ) {}

		explicit Rgba( const std::string_view p_hexa ) { fromHexaString( p_hexa ); }

		constexpr Vec4f & vec() noexcept { return _v; }

		constexpr const Vec4f & vec() const noexcept { return _v; }

		std::vector<float> toStdVector() const { return { _v.x, _v.y, _v.z, _v.w }; }

		[[nodiscard]] std::string toHexaString() const;

		[[nodiscard]] static std::string toHexaString( const Rgba & p_color );

		void fromHexaString( const std::string_view p_hexa );

		static void fromHexaString( const std::string_view p_hexa, Rgba & p_color );

		constexpr Rgba & operator+=( const Rgba & p_color ) noexcept
		{
			_v.x += p_color._v.x;
			_v.y += p_color._v.y;
			_v.z += p_color._v.z;
			_v.w += p_color._v.w;
			return *this;
		}

		constexpr Rgba & operator+=( const float p_value ) noexcept
		{
			_v.x += p_value;
			_v.y += p_value;
			_v.z += p_value;
			_v.w += p_value;
			return *this;
		}

		constexpr Rgba & operator-=( const Rgba & p_color ) noexcept
		{
			_v.x -= p_color._v.x;
			_v.y -= p_color._v.y;
			_v.z -= p_color._v.z;
			_v.w -= p_color._v.w;
			return *this;
		}

		constexpr Rgba & operator-=( const float p_value ) noexcept
		{
			_v.x -= p_value;
			_v.y -= p_value;
			_v.z -= p_value;
			_v.w -= p_value;
			return *this;
		}

		constexpr Rgba & operator*=( const Rgba & p_color ) noexcept
		{
			_v.x *= p_color._v.x;
			_v.y *= p_color._v.y;
			_v.z *= p_color._v.z;
			_v.w *= p_color._v.w;
			return *this;
		}

		constexpr Rgba & operator*=( const float p_value ) noexcept
		{
			_v.x *= p_value;
			_v.y *= p_value;
			_v.z *= p_value;
			_v.w *= p_value;
			return *this;
		}

		constexpr Rgba & operator/=( const Rgba & p_color ) noexcept
		{
			assert( p_color._v.x != 0 && p_color._v.y != 0 && p_color._v.z != 0 && p_color._v.w != 0 );
			_v.x /= p_color._v.x;
			_v.y /= p_color._v.y;
			_v.z /= p_color._v.z;
			_v.w /= p_color._v.w;
			return *this;
		}

		constexpr Rgba & operator/=( const float p_value ) noexcept
		{
			assert( p_value != 0 );
			_v.x /= p_value;
			_v.y /= p_value;
			_v.z /= p_value;
			_v.w /= p_value;
			return *this;
		}

		constexpr Rgba operator+( const Rgba & p_color ) const noexcept
		{ return Rgba( _v.x + p_color._v.x, _v.y + p_color._v.y, _v.z + p_color._v.z, _v.w + p_color._v.w ); }

		constexpr Rgba operator-( const Rgba & p_color ) const noexcept
		{ return Rgba( _v.x - p_color._v.x, _v.y - p_color._v.y, _v.z - p_color._v.z, _v.w - p_color._v.w ); }

		constexpr Rgba operator*( const Rgba & p_color ) const noexcept
		{ return Rgba( _v.x * p_color._v.x, _v.y * p_color._v.y, _v.z * p_color._v.z, _v.w * p_color._v.w ); }

		constexpr Rgba operator/( const Rgba & p_color ) const noexcept
		{
			assert( p_color._v.x != 0 && p_color._v.y != 0 && p_color._v.z != 0 && p_color._v.w != 0 );
			return Rgba( _v.x / p_color._v.x, _v.y / p_color._v.y, _v.z / p_color._v.z, _v.w / p_color._v.w );
		}

		constexpr Rgba operator+( const float p_value ) const noexcept
		{ return Rgba( _v.x + p_value, _v.y + p_value, _v.z + p_value, _v.w + p_value ); }

		constexpr Rgba operator-( const float p_value ) const noexcept
		{ return Rgba( _v.x - p_value, _v.y - p_value, _v.z - p_value, _v.w - p_value ); }

		constexpr Rgba operator*( const float p_value ) const noexcept
		{ return Rgba( _v.x * p_value, _v.y * p_value, _v.z * p_value, _v.w * p_value ); }

		constexpr Rgba operator/( const float p_value ) const noexcept
		{
			assert( p_value != 0 );
			return Rgba( _v.x / p_value, _v.y / p_value, _v.z / p_value, _v.w / p_value );
		}

		friend constexpr Rgba operator+( const float p_value, const Rgba & p_color ) noexcept
		{ return Rgba( p_value + p_color._v.x, p_value + p_color._v.y, p_value + p_color._v.z, p_value + p_color._v.w ); }

		friend constexpr Rgba operator-( const float p_value, const Rgba & p_color ) noexcept
		{ return Rgba( p_value - p_color._v.x, p_value - p_color._v.y, p_value - p_color._v.z, p_value - p_color._v.w ); }

		friend constexpr Rgba operator*( const float p_value, const Rgba & p_color ) noexcept
		{ return Rgba( p_value * p_color._v.x, p_value * p_color._v.y, p_value * p_color._v.z, p_value * p_color._v.w ); }

		friend constexpr Rgba operator/( const float p_value, const Rgba & p_color ) noexcept
		{
			assert( p_color._v.x != 0 && p_color._v.y != 0 && p_color._v.z != 0 && p_color._v.w != 0 );
			return Rgba( p_value / p_color._v.x, p_value / p_color._v.y, p_value / p_color._v.z, p_value / p_color._v.w );
		}

		constexpr bool operator==( const Rgba & p_color ) const noexcept
		{ return _v.x == p_color._v.x && _v.y == p_color._v.y && _v.z == p_color._v.z && _v.w == p_color._v.w; }

		constexpr bool operator!=( const Rgba & p_color ) const noexcept { return !( *this == p_color ); }

		friend std::ostream & operator<<( std::ostream & p_os, const Rgba & p_c )
		{
			p_os << p_c.x() << " - " << p_c.y() << " - " << p_c.z() << " - " << p_c.w() << '\n';
			return p_os;
		}

		constexpr float & x() noexcept { return _v.x; }

		constexpr float & y() noexcept { return _v.y; }

		constexpr float & z() noexcept { return _v.z; }

		constexpr float & w() noexcept { return _v.w; }

		constexpr const float & x() const noexcept { return _v.x; }

		constexpr const float & y() const noexcept { return _v.y; }

		constexpr const float & z() const noexcept { return _v.z; }

		constexpr const float & w() const noexcept { return _v.w; }

		constexpr float & r() noexcept { return _v.x; }

		constexpr float & g() noexcept { return _v.y; }

		constexpr float & b() noexcept { return _v.z; }

		constexpr float & a() noexcept { return _v.w; }

		constexpr const float & r() const noexcept { return _v.x; }

		constexpr const float & g() const noexcept { return _v.y; }

		constexpr const float & b() const noexcept { return _v.z; }

		constexpr const float & a() const noexcept { return _v.w; }

		constexpr float brightness() const noexcept { return _v.x * 0.299f + _v.y * 0.587f + _v.z * 0.114f; }

		static Rgba randomPastel() { return random() * 0.5f + 0.5f; }

		static Rgba random();

		Rgba mixedWith( const Rgba & p_other, const float p_factor ) const noexcept;
		Rgba withMinBrightness( const float p_minBrightness ) const noexcept;
		Rgba withSaturation( const float p_factor ) const noexcept;
		Rgba toLinear() const noexcept;
		Rgba toSRGB() const noexcept;
		Rgba toHighContrast() const noexcept;
		Rgba toColorblind() const noexcept;

		void saturate() noexcept;
		void applyGamma( const float & p_gamma ) noexcept;
		void oppose() noexcept;

	  private:
		Vec4f _v {};
	};
} // namespace VTX::Util::Color
