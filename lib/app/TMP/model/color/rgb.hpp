#ifndef __VTX_COLOR_RGB__
#define __VTX_COLOR_RGB__

#include <iomanip>
#include <sstream>
#include <string>
#include <util/math.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX
{
	namespace Color
	{
		class Rgb : public Vec3f
		{
		  public:
			Rgb() = default;
			explicit Rgb( const float p_r, const float p_g, const float p_b ) : Vec3f( p_r, p_g, p_b ) {}
			Rgb( const int p_r, const int p_g, const int p_b ) : Vec3f( p_r / 255.f, p_g / 255.f, p_b / 255.f ) {}
			Rgb( const Rgb & p_c ) : Vec3f( p_c.x, p_c.y, p_c.z ) {}
			explicit Rgb( const std::vector<float> & p_c )
			{
				assert( p_c.size() == 3 );
				x = p_c[ 0 ];
				y = p_c[ 1 ];
				z = p_c[ 2 ];
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

			inline void setSingleChannelHexaInStream( const float p_channelValue, std::stringstream & p_stream ) const
			{
				p_stream << std::hex << std::setw( 2 ) << std::setfill( '0' ) << (int)( p_channelValue * 255 );
			}

			inline Rgb & operator=( const Rgb & p_c )
			{
				x = p_c.x;
				y = p_c.y;
				z = p_c.z;
				return *this;
			}
			inline Rgb & operator+=( const Rgb & p_c )
			{
				x += p_c.x;
				y += p_c.y;
				z += p_c.z;
				return *this;
			}
			inline Rgb & operator+=( const float & p_f )
			{
				x += p_f;
				y += p_f;
				z += p_f;
				return *this;
			}
			inline Rgb & operator-=( const Rgb & p_c )
			{
				x -= p_c.x;
				y -= p_c.y;
				z -= p_c.z;
				return *this;
			}
			inline Rgb & operator-=( const float & p_f )
			{
				x -= p_f;
				y -= p_f;
				z -= p_f;
				return *this;
			}
			inline Rgb & operator*=( const Rgb & p_c )
			{
				x *= p_c.x;
				y *= p_c.y;
				z *= p_c.z;
				return *this;
			}
			inline Rgb & operator*=( const float & p_f )
			{
				x *= p_f;
				y *= p_f;
				z *= p_f;
				return *this;
			}
			inline Rgb & operator/=( const Rgb & p_c )
			{
				assert( p_c.x != 0.f && p_c.y != 0.f && p_c.z != 0.f );
				x /= p_c.x;
				y /= p_c.y;
				z /= p_c.z;
				return *this;
			}
			inline Rgb & operator/=( const float & p_f )
			{
				assert( p_f != 0.f );
				x /= p_f;
				y /= p_f;
				z /= p_f;
				return *this;
			}

			// TODO: check if better to reuse x= operators...
			inline Rgb operator+( const Rgb & p_c ) const { return Rgb( x + p_c.x, y + p_c.y, z + p_c.z ); }
			inline Rgb operator-( const Rgb & p_c ) const { return Rgb( x - p_c.x, y - p_c.y, z - p_c.z ); }
			inline Rgb operator*( const Rgb & p_c ) const { return Rgb( x * p_c.x, y * p_c.y, z * p_c.z ); }
			inline Rgb operator/( const Rgb & p_c ) const
			{
				assert( p_c.x != 0.f && p_c.y != 0.f && p_c.z != 0.f );
				return Rgb( x / p_c.x, y / p_c.y, z / p_c.z );
			}

			inline Rgb operator+( const float & p_f ) const { return Rgb( x + p_f, y + p_f, z + p_f ); }
			inline Rgb operator-( const float & p_f ) const { return Rgb( x - p_f, y - p_f, z - p_f ); }
			inline Rgb operator*( const float & p_f ) const { return Rgb( x * p_f, y * p_f, z * p_f ); }
			inline Rgb operator/( const float & p_f ) const
			{
				assert( p_f != 0.f );
				return Rgb( x / p_f, y / p_f, z / p_f );
			}

			friend inline Rgb operator+( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f + p_c.x, p_f + p_c.y, p_f + p_c.z );
			}
			friend inline Rgb operator-( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f - p_c.x, p_f - p_c.y, p_f - p_c.z );
			}
			friend inline Rgb operator*( const float & p_f, const Rgb & p_c )
			{
				return Rgb( p_f * p_c.x, p_f * p_c.y, p_f * p_c.z );
			}
			friend inline Rgb operator/( const float & p_f, const Rgb & p_c )
			{
				assert( p_c.x != 0.f && p_c.y != 0.f && p_c.z != 0.f );
				return Rgb( p_f / p_c.x, p_f / p_c.y, p_f / p_c.z );
			}

			inline bool operator==( const Rgb & p_c ) const { return x == p_c.x && y == p_c.y && z == p_c.z; }
			inline bool operator!=( const Rgb & p_c ) const { return x != p_c.x || y != p_c.y || z != p_c.z; }

			inline float getR() const { return x; }
			inline float getG() const { return y; }
			inline float getB() const { return z; }
			inline void	 setR( const float p_r ) { x = p_r; }
			inline void	 setG( const float p_g ) { y = p_g; }
			inline void	 setB( const float p_b ) { z = p_b; }

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

			// TODO VTX_LOGIC move this to VTX_UI_QT
			// inline QColor toQColor() const { return QColor( x * 255, y * 255, z * 255 ); }

			friend std::ostream & operator<<( std::ostream & p_os, const Rgb & p_c );

			static const Rgb BLACK;
			static const Rgb WHITE;
			static const Rgb GREY;
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
		};
	}; // namespace Color
} // namespace VTX

#endif
