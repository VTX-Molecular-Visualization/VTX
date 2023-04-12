#ifndef __VTX_COLOR_RGBA__
#define __VTX_COLOR_RGBA__

#include "util/math.hpp"
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace VTX
{
	namespace Color
	{
		class Rgba : public Vec4f
		{
		  public:
			Rgba() = default;
			Rgba( const float p_r, const float p_g, const float p_b, const float p_a ) : Vec4f( p_r, p_g, p_b, p_a ) {}
			Rgba( const float p_r, const float p_g, const float p_b ) : Vec4f( p_r, p_g, p_b, 1.f ) {}
			Rgba( const int p_r, const int p_g, const int p_b, const int p_a ) :
				Vec4f( p_r / 255.f, p_g / 255.f, p_b / 255.f, p_a / 255.f )
			{
			}
			Rgba( const int p_r, const int p_g, const int p_b ) : Vec4f( p_r / 255.f, p_g / 255.f, p_b / 255.f, 1.f ) {}
			Rgba( const Rgba & p_c ) : Vec4f( p_c.x, p_c.y, p_c.z, p_c.w ) {}
			explicit Rgba( const std::vector<float> & p_c )
			{
				assert( p_c.size() == 4 );
				x = p_c[ 0 ];
				y = p_c[ 1 ];
				z = p_c[ 2 ];
				w = p_c[ 3 ];
			}

			inline std::vector<float> toStdVector() const { return { x, y, z }; }
			inline std::string		  toHexaString() const
			{
				std::stringstream stringstream;

				stringstream << "#";
				setSingleChannelHexaInStream( x, stringstream );
				setSingleChannelHexaInStream( y, stringstream );
				setSingleChannelHexaInStream( z, stringstream );

				return stringstream.str();
			};
			inline std::string toHexaStringAlpha() const
			{
				std::stringstream stringstream;

				stringstream << "#";
				setSingleChannelHexaInStream( x, stringstream );
				setSingleChannelHexaInStream( y, stringstream );
				setSingleChannelHexaInStream( z, stringstream );
				setSingleChannelHexaInStream( w, stringstream );

				return stringstream.str();
			};

			inline void setSingleChannelHexaInStream( const float p_channelValue, std::stringstream & p_stream ) const
			{
				p_stream << std::hex << std::setw( 2 ) << std::setfill( '0' ) << (int)( p_channelValue * 255 );
			}

			inline Rgba & operator=( const Rgba & p_c )
			{
				x = p_c.x;
				y = p_c.y;
				z = p_c.z;
				w = p_c.w;
				return *this;
			}
			inline Rgba & operator+=( const Rgba & p_c )
			{
				x += p_c.x;
				y += p_c.y;
				z += p_c.z;
				return *this;
			}
			inline Rgba & operator+=( const float & p_f )
			{
				x += p_f;
				y += p_f;
				z += p_f;
				return *this;
			}
			inline Rgba & operator-=( const Rgba & p_c )
			{
				x -= p_c.x;
				y -= p_c.y;
				z -= p_c.z;
				return *this;
			}
			inline Rgba & operator-=( const float & p_f )
			{
				x -= p_f;
				y -= p_f;
				z -= p_f;
				return *this;
			}
			inline Rgba & operator*=( const Rgba & p_c )
			{
				x *= p_c.x;
				y *= p_c.y;
				z *= p_c.z;
				return *this;
			}
			inline Rgba & operator*=( const float & p_f )
			{
				x *= p_f;
				y *= p_f;
				z *= p_f;
				return *this;
			}
			inline Rgba & operator/=( const Rgba & p_c )
			{
				assert( p_c.x != 0.f && p_c.y != 0.f && p_c.z != 0.f );
				x /= p_c.x;
				y /= p_c.y;
				z /= p_c.z;
				return *this;
			}
			inline Rgba & operator/=( const float & p_f )
			{
				assert( p_f != 0.f );
				x /= p_f;
				y /= p_f;
				z /= p_f;
				return *this;
			}

			// TODO: check if better to reuse x= operators...
			inline Rgba operator+( const Rgba & p_c ) const { return Rgba( x + p_c.x, y + p_c.y, z + p_c.z ); }
			inline Rgba operator-( const Rgba & p_c ) const { return Rgba( x - p_c.x, y - p_c.y, z - p_c.z ); }
			inline Rgba operator*( const Rgba & p_c ) const { return Rgba( x * p_c.x, y * p_c.y, z * p_c.z ); }
			inline Rgba operator/( const Rgba & p_c ) const
			{
				assert( p_c.x != 0.f && p_c.y != 0.f && p_c.z != 0.f );
				return Rgba( x / p_c.x, y / p_c.y, z / p_c.z );
			}

			inline Rgba operator+( const float & p_f ) const { return Rgba( x + p_f, y + p_f, z + p_f ); }
			inline Rgba operator-( const float & p_f ) const { return Rgba( x - p_f, y - p_f, z - p_f ); }
			inline Rgba operator*( const float & p_f ) const { return Rgba( x * p_f, y * p_f, z * p_f ); }
			inline Rgba operator/( const float & p_f ) const
			{
				assert( p_f != 0.f );
				return Rgba( x / p_f, y / p_f, z / p_f );
			}

			friend inline Rgba operator+( const float & p_f, const Rgba & p_c )
			{
				return Rgba( p_f + p_c.x, p_f + p_c.y, p_f + p_c.z );
			}
			friend inline Rgba operator-( const float & p_f, const Rgba & p_c )
			{
				return Rgba( p_f - p_c.x, p_f - p_c.y, p_f - p_c.z );
			}
			friend inline Rgba operator*( const float & p_f, const Rgba & p_c )
			{
				return Rgba( p_f * p_c.x, p_f * p_c.y, p_f * p_c.z );
			}
			friend inline Rgba operator/( const float & p_f, const Rgba & p_c )
			{
				assert( p_c.x != 0.f && p_c.y != 0.f && p_c.z != 0.f );
				return Rgba( p_f / p_c.x, p_f / p_c.y, p_f / p_c.z );
			}

			inline bool operator==( const Rgba & p_c ) const { return x == p_c.x && y == p_c.y && z == p_c.z; }
			inline bool operator!=( const Rgba & p_c ) const { return x != p_c.x || y != p_c.y || z != p_c.z; }

			inline float getR() const { return x; }
			inline float getG() const { return y; }
			inline float getB() const { return z; }
			inline float getA() const { return w; }
			inline void	 setR( const float p_r ) { x = p_r; }
			inline void	 setG( const float p_g ) { y = p_g; }
			inline void	 setB( const float p_b ) { z = p_b; }
			inline void	 setA( const float p_a ) { w = p_a; }

			inline void saturate()
			{
				x = Util::Math::clamp( x, 0.f, 1.f );
				y = Util::Math::clamp( y, 0.f, 1.f );
				z = Util::Math::clamp( z, 0.f, 1.f );
			}

			inline void applyGamma( const float & pyamma )
			{
				x = powf( x, pyamma );
				y = powf( y, pyamma );
				z = powf( z, pyamma );
			}

			inline void oppose()
			{
				x = 1.f - x;
				y = 1.f - y;
				z = 1.f - z;
			}

			inline const float brightness() const { return ( x * 0.299f ) + ( y * 0.587f ) + ( z * 0.114f ); }

			friend std::ostream & operator<<( std::ostream & p_os, const Rgba & p_c );

			static const Rgba BLACK;
			static const Rgba WHITE;
			static const Rgba GREY;
			static const Rgba RED;
			static const Rgba GREEN;
			static const Rgba BLUE;
			static const Rgba YELLOW;
			static const Rgba MAGENTA;
			static const Rgba CYAN;

			static inline Rgba random()
			{
				return Rgba( Util::Math::randomFloat(), Util::Math::randomFloat(), Util::Math::randomFloat() );
			}

			static inline Rgba randomPastel() { return random() * 0.5f + 0.5f; }
		};
	}; // namespace Color
} // namespace VTX

#endif
