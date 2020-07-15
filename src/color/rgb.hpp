#ifndef __VTX_RGB__
#define __VTX_RGB__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_serializable.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Color
	{
		class Rgb : public Generic::BaseSerializable
		{
		  public:
			Rgb() = default;
			explicit Rgb( const float p_r, const float p_g, const float p_b )

			{
				_rgb[ 0 ] = p_r;
				_rgb[ 1 ] = p_g;
				_rgb[ 2 ] = p_b;
			}
			Rgb( const int p_r, const int p_g, const int p_b )
			{
				_rgb[ 0 ] = p_r / 255.f;
				_rgb[ 1 ] = p_g / 255.f;
				_rgb[ 2 ] = p_b / 255.f;
			}
			Rgb( const Rgb & p_c )
			{
				_rgb[ 0 ] = p_c._rgb[ 0 ];
				_rgb[ 1 ] = p_c._rgb[ 1 ];
				_rgb[ 2 ] = p_c._rgb[ 2 ];
			}

			inline Rgb & operator=( const Rgb & p_c )
			{
				_rgb[ 0 ] = p_c._rgb[ 0 ];
				_rgb[ 1 ] = p_c._rgb[ 1 ];
				_rgb[ 2 ] = p_c._rgb[ 2 ];
				return *this;
			}
			inline Rgb & operator+=( const Rgb & p_c )
			{
				_rgb[ 0 ] += p_c._rgb[ 0 ];
				_rgb[ 1 ] += p_c._rgb[ 1 ];
				_rgb[ 2 ] += p_c._rgb[ 2 ];
				return *this;
			}
			inline Rgb & operator+=( const float & p_f )
			{
				_rgb[ 0 ] += p_f;
				_rgb[ 1 ] += p_f;
				_rgb[ 2 ] += p_f;
				return *this;
			}
			inline Rgb & operator-=( const Rgb & p_c )
			{
				_rgb[ 0 ] -= p_c._rgb[ 0 ];
				_rgb[ 1 ] -= p_c._rgb[ 1 ];
				_rgb[ 2 ] -= p_c._rgb[ 2 ];
				return *this;
			}
			inline Rgb & operator-=( const float & p_f )
			{
				_rgb[ 0 ] -= p_f;
				_rgb[ 1 ] -= p_f;
				_rgb[ 2 ] -= p_f;
				return *this;
			}
			inline Rgb & operator*=( const Rgb & p_c )
			{
				_rgb[ 0 ] *= p_c._rgb[ 0 ];
				_rgb[ 1 ] *= p_c._rgb[ 1 ];
				_rgb[ 2 ] *= p_c._rgb[ 2 ];
				return *this;
			}
			inline Rgb & operator*=( const float & p_f )
			{
				_rgb[ 0 ] *= p_f;
				_rgb[ 1 ] *= p_f;
				_rgb[ 2 ] *= p_f;
				return *this;
			}
			inline Rgb & operator/=( const Rgb & p_c )
			{
				assert( p_c._rgb[ 0 ] != 0.f && p_c._rgb[ 1 ] != 0.f && p_c._rgb[ 2 ] != 0.f );
				_rgb[ 0 ] /= p_c._rgb[ 0 ];
				_rgb[ 1 ] /= p_c._rgb[ 1 ];
				_rgb[ 2 ] /= p_c._rgb[ 2 ];
				return *this;
			}
			inline Rgb & operator/=( const float & p_f )
			{
				assert( p_f != 0.f );
				_rgb[ 0 ] /= p_f;
				_rgb[ 1 ] /= p_f;
				_rgb[ 2 ] /= p_f;
				return *this;
			}

			// TODO: check if better to reuse x= operators...
			inline Rgb operator+( const Rgb & p_c ) const
			{
				return Rgb( _rgb[ 0 ] + p_c._rgb[ 0 ], _rgb[ 1 ] + p_c._rgb[ 1 ], _rgb[ 2 ] + p_c._rgb[ 2 ] );
			}
			inline Rgb operator-( const Rgb & p_c ) const
			{
				return Rgb( _rgb[ 0 ] - p_c._rgb[ 0 ], _rgb[ 1 ] - p_c._rgb[ 1 ], _rgb[ 2 ] - p_c._rgb[ 2 ] );
			}
			inline Rgb operator*( const Rgb & p_c ) const
			{
				return Rgb( _rgb[ 0 ] * p_c._rgb[ 0 ], _rgb[ 1 ] * p_c._rgb[ 1 ], _rgb[ 2 ] * p_c._rgb[ 2 ] );
			}
			inline Rgb operator/( const Rgb & p_c ) const
			{
				assert( p_c._rgb[ 0 ] != 0.f && p_c._rgb[ 1 ] != 0.f && p_c._rgb[ 2 ] != 0.f );
				return Rgb( _rgb[ 0 ] / p_c._rgb[ 0 ], _rgb[ 1 ] / p_c._rgb[ 1 ], _rgb[ 2 ] / p_c._rgb[ 2 ] );
			}

			inline Rgb operator+( const float & p_f ) const
			{
				return Rgb( _rgb[ 0 ] + p_f, _rgb[ 1 ] + p_f, _rgb[ 2 ] + p_f );
			}
			inline Rgb operator-( const float & p_f ) const
			{
				return Rgb( _rgb[ 0 ] - p_f, _rgb[ 1 ] - p_f, _rgb[ 2 ] - p_f );
			}
			inline Rgb operator*( const float & p_f ) const
			{
				return Rgb( _rgb[ 0 ] * p_f, _rgb[ 1 ] * p_f, _rgb[ 2 ] * p_f );
			}
			inline Rgb operator/( const float & p_f ) const
			{
				assert( p_f != 0.f );
				return Rgb( _rgb[ 0 ] / p_f, _rgb[ 1 ] / p_f, _rgb[ 2 ] / p_f );
			}

			friend inline Rgb operator+( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f + p_c._rgb[ 0 ], p_f + p_c._rgb[ 1 ], p_f + p_c._rgb[ 2 ] );
			}
			friend inline Rgb operator-( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f - p_c._rgb[ 0 ], p_f - p_c._rgb[ 1 ], p_f - p_c._rgb[ 2 ] );
			}
			friend inline Rgb operator*( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f * p_c._rgb[ 0 ], p_f * p_c._rgb[ 1 ], p_f * p_c._rgb[ 2 ] );
			}
			friend inline Rgb operator/( const float & p_f, const Rgb & p_c )
			{
				assert( p_c._rgb[ 0 ] != 0.f && p_c._rgb[ 1 ] != 0.f && p_c._rgb[ 2 ] != 0.f );
				return Rgb( p_f / p_c._rgb[ 0 ], p_f / p_c._rgb[ 1 ], p_f / p_c._rgb[ 2 ] );
			}

			inline bool operator==( const Rgb & p_c )
			{
				return _rgb[ 0 ] == p_c._rgb[ 0 ] && _rgb[ 1 ] == p_c._rgb[ 1 ] && _rgb[ 2 ] == p_c._rgb[ 2 ];
			}
			inline bool operator!=( const Rgb & p_c )
			{
				return _rgb[ 0 ] != p_c._rgb[ 0 ] && _rgb[ 1 ] != p_c._rgb[ 1 ] && _rgb[ 2 ] != p_c._rgb[ 2 ];
			}

			inline const float * const getRGB() const { return _rgb; }
			inline float * const	   getRGB() { return _rgb; }
			inline float			   getR() const { return _rgb[ 0 ]; }
			inline float			   getG() const { return _rgb[ 1 ]; }
			inline float			   getB() const { return _rgb[ 2 ]; }
			inline void				   setR( const float p_r ) { _rgb[ 0 ] = p_r; }
			inline void				   setG( const float p_g ) { _rgb[ 1 ] = p_g; }
			inline void				   setB( const float p_b ) { _rgb[ 2 ] = p_b; }

			inline void saturate()
			{
				_rgb[ 0 ] = Util::Math::clamp( _rgb[ 0 ], 0.f, 1.f );
				_rgb[ 1 ] = Util::Math::clamp( _rgb[ 1 ], 0.f, 1.f );
				_rgb[ 2 ] = Util::Math::clamp( _rgb[ 2 ], 0.f, 1.f );
			}

			inline void applyGamma( const float & p_gamma )
			{
				_rgb[ 0 ] = powf( _rgb[ 0 ], p_gamma );
				_rgb[ 1 ] = powf( _rgb[ 1 ], p_gamma );
				_rgb[ 2 ] = powf( _rgb[ 2 ], p_gamma );
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

			virtual void fromJson( nlohmann::json & p_json ) override
			{
				_rgb[ 0 ] = p_json[ "R" ].get<float>();
				_rgb[ 1 ] = p_json[ "G" ].get<float>();
				_rgb[ 2 ] = p_json[ "B" ].get<float>();
			}
			virtual nlohmann::json toJson() const override
			{
				return { { "R", _rgb[ 0 ] }, { "G", _rgb[ 1 ] }, { "B", _rgb[ 2 ] } };
			}

		  private:
			float _rgb[ 3 ] = { 0.f, 0.f, 0.f };
		};
	}; // namespace Color
} // namespace VTX

#endif
