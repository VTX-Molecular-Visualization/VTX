#ifndef __VTX_RGB__
#define __VTX_RGB__

#ifdef _MSC_VER
#pragma once
#endif

#include "util/math.hpp"

namespace VTX
{
	namespace Color
	{
		class Rgb
		{
		  public:
			Rgb() = default;
			explicit Rgb( const float p_r, const float p_g, const float p_b ) : _r( p_r ), _g( p_g ), _b( p_b ) {}
			Rgb( const int p_r, const int p_g, const int p_b ) : _r( p_r / 255.f ), _g( p_g / 255.f ), _b( p_b / 255.f )
			{
			}
			Rgb( const Rgb & p_c ) : _r( p_c._r ), _g( p_c._g ), _b( p_c._b ) {}
			explicit Rgb( const std::vector<float> & p_c )
			{
				assert( p_c.size() == 3 );
				_r = p_c[ 0 ];
				_g = p_c[ 1 ];
				_b = p_c[ 2 ];
			}

			inline std::vector<float> toStdVector() const { return { _r, _g, _b }; }

			inline Rgb & operator=( const Rgb & p_c )
			{
				_r = p_c._r;
				_g = p_c._g;
				_b = p_c._b;
				return *this;
			}
			inline Rgb & operator+=( const Rgb & p_c )
			{
				_r += p_c._r;
				_g += p_c._g;
				_b += p_c._b;
				return *this;
			}
			inline Rgb & operator+=( const float & p_f )
			{
				_r += p_f;
				_g += p_f;
				_b += p_f;
				return *this;
			}
			inline Rgb & operator-=( const Rgb & p_c )
			{
				_r -= p_c._r;
				_g -= p_c._g;
				_b -= p_c._b;
				return *this;
			}
			inline Rgb & operator-=( const float & p_f )
			{
				_r -= p_f;
				_g -= p_f;
				_b -= p_f;
				return *this;
			}
			inline Rgb & operator*=( const Rgb & p_c )
			{
				_r *= p_c._r;
				_g *= p_c._g;
				_b *= p_c._b;
				return *this;
			}
			inline Rgb & operator*=( const float & p_f )
			{
				_r *= p_f;
				_g *= p_f;
				_b *= p_f;
				return *this;
			}
			inline Rgb & operator/=( const Rgb & p_c )
			{
				assert( p_c._r != 0.f && p_c._g != 0.f && p_c._b != 0.f );
				_r /= p_c._r;
				_g /= p_c._g;
				_b /= p_c._b;
				return *this;
			}
			inline Rgb & operator/=( const float & p_f )
			{
				assert( p_f != 0.f );
				_r /= p_f;
				_g /= p_f;
				_b /= p_f;
				return *this;
			}

			// TODO: check if better to reuse x= operators...
			inline Rgb operator+( const Rgb & p_c ) const { return Rgb( _r + p_c._r, _g + p_c._g, _b + p_c._b ); }
			inline Rgb operator-( const Rgb & p_c ) const { return Rgb( _r - p_c._r, _g - p_c._g, _b - p_c._b ); }
			inline Rgb operator*( const Rgb & p_c ) const { return Rgb( _r * p_c._r, _g * p_c._g, _b * p_c._b ); }
			inline Rgb operator/( const Rgb & p_c ) const
			{
				assert( p_c._r != 0.f && p_c._g != 0.f && p_c._b != 0.f );
				return Rgb( _r / p_c._r, _g / p_c._g, _b / p_c._b );
			}

			inline Rgb operator+( const float & p_f ) const { return Rgb( _r + p_f, _g + p_f, _b + p_f ); }
			inline Rgb operator-( const float & p_f ) const { return Rgb( _r - p_f, _g - p_f, _b - p_f ); }
			inline Rgb operator*( const float & p_f ) const { return Rgb( _r * p_f, _g * p_f, _b * p_f ); }
			inline Rgb operator/( const float & p_f ) const
			{
				assert( p_f != 0.f );
				return Rgb( _r / p_f, _g / p_f, _b / p_f );
			}

			friend inline Rgb operator+( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f + p_c._r, p_f + p_c._g, p_f + p_c._b );
			}
			friend inline Rgb operator-( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f - p_c._r, p_f - p_c._g, p_f - p_c._b );
			}
			friend inline Rgb operator*( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f * p_c._r, p_f * p_c._g, p_f * p_c._b );
			}
			friend inline Rgb operator/( const float & p_f, const Rgb & p_c )
			{
				assert( p_c._r != 0.f && p_c._g != 0.f && p_c._b != 0.f );
				return Rgb( p_f / p_c._r, p_f / p_c._g, p_f / p_c._b );
			}

			inline bool operator==( const Rgb & p_c ) { return _r == p_c._r && _g == p_c._g && _b == p_c._b; }
			inline bool operator!=( const Rgb & p_c ) { return _r != p_c._r && _g != p_c._g && _b != p_c._b; }

			inline float getR() const { return _r; }
			inline float getG() const { return _g; }
			inline float getB() const { return _b; }
			inline void	 setR( const float p_r ) { _r = p_r; }
			inline void	 setG( const float p_g ) { _g = p_g; }
			inline void	 setB( const float p_b ) { _b = p_b; }

			inline void saturate()
			{
				_r = Util::Math::clamp( _r, 0.f, 1.f );
				_g = Util::Math::clamp( _g, 0.f, 1.f );
				_b = Util::Math::clamp( _b, 0.f, 1.f );
			}

			inline void applyGamma( const float & p_gamma )
			{
				_r = powf( _r, p_gamma );
				_g = powf( _g, p_gamma );
				_b = powf( _b, p_gamma );
			}

			friend std::ostream & operator<<( std::ostream & p_os, const Rgb & p_c );

			static const Rgb BLACK;
			static const Rgb WHITE;
			static const Rgb RED;
			static const Rgb GREEN;
			static const Rgb BLUE;
			static const Rgb YELLOW;
			static const Rgb MAGENTA;
			static const Rgb CYAN;

			static inline Rgb random()
			{
				return Rgb( Util::Math::randomFloat(), Util::Math::randomFloat(), Util::Math::randomFloat() );
			}
			static inline Rgb randomPastel() { return random() * 0.5f + 0.5f; }

			void fromJson( nlohmann::json & p_json )
			{
				_r = p_json[ "R" ].get<float>();
				_g = p_json[ "G" ].get<float>();
				_b = p_json[ "B" ].get<float>();
			}
			nlohmann::json toJson() const { return { { "R", _r }, { "G", _g }, { "B", _b } }; }

		  private:
			float _r = 0.f;
			float _g = 0.f;
			float _b = 0.f;
		};
	}; // namespace Color
} // namespace VTX

#endif
