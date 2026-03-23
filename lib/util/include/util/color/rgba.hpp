#ifndef __VTX_UTIL_COLOR_RGBA__
#define __VTX_UTIL_COLOR_RGBA__

#include "util/types.hpp"
#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace VTX::Util::Color
{
	/**
	 * @brief Color RGBA (glm::vec4 wrapper).
	 */
	class Rgba
	{
	  public:
		constexpr Rgba() = default;

		constexpr Rgba( float p_r, float p_g, float p_b, float p_a ) : _v( p_r, p_g, p_b, p_a ) {}

		constexpr Rgba( float p_r, float p_g, float p_b ) : _v( p_r, p_g, p_b, 1.f ) {}

		constexpr Rgba( int p_r, int p_g, int p_b, int p_a ) : _v( p_r / 255.f, p_g / 255.f, p_b / 255.f, p_a / 255.f )
		{
		}

		constexpr Rgba( int p_r, int p_g, int p_b ) : _v( p_r / 255.f, p_g / 255.f, p_b / 255.f, 1.f ) {}

		constexpr Rgba( const Rgba & ) = default;

		explicit Rgba( const std::vector<float> & p_c )
		{
			assert( p_c.size() == 4 );
			_v.x = p_c[ 0 ];
			_v.y = p_c[ 1 ];
			_v.z = p_c[ 2 ];
			_v.w = p_c[ 3 ];
		}

		constexpr explicit Rgba( const Vec4f & p_v ) : _v( p_v ) {}

		constexpr Vec4f &		vec() { return _v; }
		constexpr const Vec4f & vec() const { return _v; }

		constexpr operator Vec4f &() { return _v; }
		constexpr operator const Vec4f &() const { return _v; }

		inline std::vector<float> toStdVector() const { return { _v.x, _v.y, _v.z }; }

		inline std::string toHexaString() const
		{
			std::stringstream ss;
			ss << "#";
			setSingleChannelHexaInStream( _v.x, ss );
			setSingleChannelHexaInStream( _v.y, ss );
			setSingleChannelHexaInStream( _v.z, ss );
			return ss.str();
		}

		inline std::string toHexaStringAlpha() const
		{
			std::stringstream ss;
			ss << "#";
			setSingleChannelHexaInStream( _v.x, ss );
			setSingleChannelHexaInStream( _v.y, ss );
			setSingleChannelHexaInStream( _v.z, ss );
			setSingleChannelHexaInStream( _v.w, ss );
			return ss.str();
		}

		inline void setSingleChannelHexaInStream( float v, std::stringstream & ss ) const
		{
			ss << std::hex << std::setw( 2 ) << std::setfill( '0' ) << (int)( v * 255 );
		}

		constexpr Rgba & operator=( const Rgba & ) = default;

		constexpr Rgba & operator+=( const Rgba & p )
		{
			_v.x += p._v.x;
			_v.y += p._v.y;
			_v.z += p._v.z;
			return *this;
		}

		constexpr Rgba & operator+=( float f )
		{
			_v.x += f;
			_v.y += f;
			_v.z += f;
			return *this;
		}

		constexpr Rgba & operator-=( const Rgba & p )
		{
			_v.x -= p._v.x;
			_v.y -= p._v.y;
			_v.z -= p._v.z;
			return *this;
		}

		constexpr Rgba & operator-=( float f )
		{
			_v.x -= f;
			_v.y -= f;
			_v.z -= f;
			return *this;
		}

		constexpr Rgba & operator*=( const Rgba & p )
		{
			_v.x *= p._v.x;
			_v.y *= p._v.y;
			_v.z *= p._v.z;
			return *this;
		}

		constexpr Rgba & operator*=( float f )
		{
			_v.x *= f;
			_v.y *= f;
			_v.z *= f;
			return *this;
		}

		constexpr Rgba & operator/=( const Rgba & p )
		{
			assert( p._v.x != 0 && p._v.y != 0 && p._v.z != 0 );
			_v.x /= p._v.x;
			_v.y /= p._v.y;
			_v.z /= p._v.z;
			return *this;
		}

		constexpr Rgba & operator/=( float f )
		{
			assert( f != 0 );
			_v.x /= f;
			_v.y /= f;
			_v.z /= f;
			return *this;
		}

		constexpr Rgba operator+( const Rgba & p ) const { return Rgba( _v.x + p._v.x, _v.y + p._v.y, _v.z + p._v.z ); }
		constexpr Rgba operator-( const Rgba & p ) const { return Rgba( _v.x - p._v.x, _v.y - p._v.y, _v.z - p._v.z ); }
		constexpr Rgba operator*( const Rgba & p ) const { return Rgba( _v.x * p._v.x, _v.y * p._v.y, _v.z * p._v.z ); }

		constexpr Rgba operator/( const Rgba & p ) const
		{
			assert( p._v.x != 0 && p._v.y != 0 && p._v.z != 0 );
			return Rgba( _v.x / p._v.x, _v.y / p._v.y, _v.z / p._v.z );
		}

		constexpr Rgba operator+( float f ) const { return Rgba( _v.x + f, _v.y + f, _v.z + f ); }
		constexpr Rgba operator-( float f ) const { return Rgba( _v.x - f, _v.y - f, _v.z - f ); }
		constexpr Rgba operator*( float f ) const { return Rgba( _v.x * f, _v.y * f, _v.z * f ); }

		constexpr Rgba operator/( float f ) const
		{
			assert( f != 0 );
			return Rgba( _v.x / f, _v.y / f, _v.z / f );
		}

		friend constexpr Rgba operator+( float f, const Rgba & p )
		{
			return Rgba( f + p._v.x, f + p._v.y, f + p._v.z );
		}

		friend constexpr Rgba operator-( float f, const Rgba & p )
		{
			return Rgba( f - p._v.x, f - p._v.y, f - p._v.z );
		}

		friend constexpr Rgba operator*( float f, const Rgba & p )
		{
			return Rgba( f * p._v.x, f * p._v.y, f * p._v.z );
		}

		friend constexpr Rgba operator/( float f, const Rgba & p )
		{
			assert( p._v.x != 0 && p._v.y != 0 && p._v.z != 0 );
			return Rgba( f / p._v.x, f / p._v.y, f / p._v.z );
		}

		constexpr bool operator==( const Rgba & p ) const
		{
			return _v.x == p._v.x && _v.y == p._v.y && _v.z == p._v.z && _v.w == p._v.w;
		}

		constexpr bool operator!=( const Rgba & p ) const { return !( *this == p ); }

		constexpr float & x() { return _v.x; }
		constexpr float & y() { return _v.y; }
		constexpr float & z() { return _v.z; }
		constexpr float & w() { return _v.w; }

		constexpr const float & x() const { return _v.x; }
		constexpr const float & y() const { return _v.y; }
		constexpr const float & z() const { return _v.z; }
		constexpr const float & w() const { return _v.w; }

		constexpr float & r() { return _v.x; }
		constexpr float & g() { return _v.y; }
		constexpr float & b() { return _v.z; }
		constexpr float & a() { return _v.w; }

		constexpr const float & r() const { return _v.x; }
		constexpr const float & g() const { return _v.y; }
		constexpr const float & b() const { return _v.z; }
		constexpr const float & a() const { return _v.w; }

		constexpr float brightness() const { return _v.x * 0.299f + _v.y * 0.587f + _v.z * 0.114f; }

		static Rgba randomPastel() { return random() * 0.5f + 0.5f; }
		static Rgba random();

		Rgba mixedWith( const Rgba &, const float ) const;
		Rgba withMinBrightness( const float ) const;
		Rgba withSaturation( const float ) const;
		Rgba toHighContrast() const;
		Rgba toColorblind() const;

		void saturate();
		void applyGamma( const float & );
		void oppose();

	  private:
		Vec4f _v {};
	};
} // namespace VTX::Util::Color

#endif
