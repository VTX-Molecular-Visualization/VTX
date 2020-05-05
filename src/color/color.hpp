#ifndef __VTX_COLOR__
#define __VTX_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "util/math.hpp"

namespace VTX
{
	struct Color
	{
		Color() = default;
		Color( const float p_r, const float p_g, const float p_b ) : _r( p_r ), _g( p_g ), _b( p_b ) {}
		Color( const int p_r, const int p_g, const int p_b ) : _r( p_r / 255.f ), _g( p_g / 255.f ), _b( p_b / 255.f )
		{
		}
		Color( const Color & p_c ) : _r( p_c._r ), _g( p_c._g ), _b( p_c._b ) {}

		inline Color & operator=( const Color & p_c )
		{
			_r = p_c._r;
			_g = p_c._g;
			_b = p_c._b;
			return *this;
		}
		inline Color & operator+=( const Color & p_c )
		{
			_r += p_c._r;
			_g += p_c._g;
			_b += p_c._b;
			return *this;
		}
		inline Color & operator+=( const float & p_f )
		{
			_r += p_f;
			_g += p_f;
			_b += p_f;
			return *this;
		}
		inline Color & operator-=( const Color & p_c )
		{
			_r -= p_c._r;
			_g -= p_c._g;
			_b -= p_c._b;
			return *this;
		}
		inline Color & operator-=( const float & p_f )
		{
			_r -= p_f;
			_g -= p_f;
			_b -= p_f;
			return *this;
		}
		inline Color & operator*=( const Color & p_c )
		{
			_r *= p_c._r;
			_g *= p_c._g;
			_b *= p_c._b;
			return *this;
		}
		inline Color & operator*=( const float & p_f )
		{
			_r *= p_f;
			_g *= p_f;
			_b *= p_f;
			return *this;
		}
		inline Color & operator/=( const Color & p_c )
		{
			assert( p_c._r != 0.f && p_c._g != 0.f && p_c._b != 0.f );
			_r /= p_c._r;
			_g /= p_c._g;
			_b /= p_c._b;
			return *this;
		}
		inline Color & operator/=( const float & p_f )
		{
			assert( p_f != 0.f );
			_r /= p_f;
			_g /= p_f;
			_b /= p_f;
			return *this;
		}

		// TODO: check if better to reuse x= operators...
		inline Color operator+( const Color & p_c ) const { return Color( _r + p_c._r, _g + p_c._g, _b + p_c._b ); }
		inline Color operator+( const float & p_f ) const { return Color( _r + p_f, _g + p_f, _b + p_f ); }
		inline Color operator-( const Color & p_c ) const { return Color( _r - p_c._r, _g - p_c._g, _b - p_c._b ); }
		inline Color operator-( const float & p_f ) const { return Color( _r - p_f, _g - p_f, _b - p_f ); }
		inline Color operator*( const Color & p_c ) const { return Color( _r * p_c._r, _g * p_c._g, _b * p_c._b ); }
		inline Color operator*( const float & p_f ) const { return Color( _r * p_f, _g * p_f, _b * p_f ); }
		inline Color operator/( const Color & p_c ) const
		{
			assert( p_c._r != 0.f && p_c._g != 0.f && p_c._b != 0.f );
			return Color( _r / p_c._r, _g / p_c._g, _b / p_c._b );
		}
		inline Color operator/( const float & p_f ) const
		{
			assert( p_f != 0.f );
			return Color( _r / p_f, _g / p_f, _b / p_f );
		}

		friend inline Color operator+( const float & p_f, const Color & p_c )
		{
			return Color( p_f + p_c._r, p_f + p_c._g, p_f + p_c._b );
		}
		friend inline Color operator-( const float & p_f, const Color & p_c )
		{
			return Color( p_f - p_c._r, p_f - p_c._g, p_f - p_c._b );
		}
		friend inline Color operator*( const float & p_f, const Color & p_c )
		{
			return Color( p_f * p_c._r, p_f * p_c._g, p_f * p_c._b );
		}
		friend inline Color operator/( const float & p_f, const Color & p_c )
		{
			assert( p_c._r != 0.f && p_c._g != 0.f && p_c._b != 0.f );
			return Color( p_f / p_c._r, p_f / p_c._g, p_f / p_c._b );
		}

		inline bool operator==( const Color & p_c ) { return _r == p_c._r && _g == p_c._g && _b == p_c._b; }
		inline bool operator!=( const Color & p_c ) { return _r != p_c._r && _g != p_c._g && _b != p_c._b; }

		inline void saturate()
		{
			_r = Util::Math::clamp( _r, 0.f, 1.f );
			_g = Util::Math::clamp( _r, 0.f, 1.f );
			_b = Util::Math::clamp( _r, 0.f, 1.f );
		}

		inline void applyGamma( const float & p_gamma )
		{
			_r = powf( _r, p_gamma );
			_g = powf( _b, p_gamma );
			_b = powf( _b, p_gamma );
		}

		static const Color black;
		static const Color white;
		static const Color red;
		static const Color green;
		static const Color blue;
		static const Color yellow;
		static const Color magenta;
		static const Color cyan;

		static const std::vector<Color> predefinedColors;

		static inline Color random()
		{
			return Color( Util::Math::randomFloat(), Util::Math::randomFloat(), Util::Math::randomFloat() );
		}
		static inline Color randomPastel() { return random() * 0.5f + 0.5f; }

		float _r = 0.f;
		float _g = 0.f;
		float _b = 0.f;
	};
} // namespace VTX

#endif
