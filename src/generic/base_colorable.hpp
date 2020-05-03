#ifndef __VTX_BASE_COLORABLE__
#define __VTX_BASE_COLORABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "util/color.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseColorable
		{
		  public:
			inline const Vec3f & getColor() const { return _color; }
			inline void			 setColor( const Vec3f & p_color ) { _color = p_color; }

		  protected:
			Vec3f _color = Util::Color::randomPastel();
		};
	} // namespace Generic
} // namespace VTX
#endif
