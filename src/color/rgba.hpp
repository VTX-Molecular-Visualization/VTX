#ifndef __VTX_RGBA__
#define __VTX_RGBA__

#ifdef _MSC_VER
#pragma once
#endif

#include "rgb.hpp"

namespace VTX
{
	namespace Color
	{
		class Rgba : public Rgb
		{
		  public:
			Rgba() = default;
			explicit Rgba( const float p_r, const float p_g, const float p_b, const float p_a ) :
				Rgb( p_r, p_g, p_b ), _a( p_a )
			{
			}
			Rgba( const int p_r, const int p_g, const int p_b, const int p_a ) : Rgb( p_r, p_g, p_b ), _a( p_a / 255.f )
			{
			}
			Rgba( const Rgba & p_c ) : Rgb( p_c ), _a( p_c._a ) {}
			explicit Rgba( const std::vector<float> & p_c )
			{
				assert( p_c.size() == 4 );
				_r = p_c[ 0 ];
				_g = p_c[ 1 ];
				_b = p_c[ 2 ];
				_a = p_c[ 3 ];
			}

			inline float getA() const { return _a; }
			inline void	 setA( const float p_a ) { _a = p_a; }

			inline QColor toQColor() const override { return QColor( _r * 255, _g * 255, _b * 255, 255 ); }

		  protected:
			float _a = 0.f;
		};
	} // namespace Color
} // namespace VTX

#endif
