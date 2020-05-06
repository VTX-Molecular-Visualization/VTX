#ifndef __VTX_BASE_COLORABLE__
#define __VTX_BASE_COLORABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/color.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseColorable
		{
		  public:
			inline const Color::Rgb & getColor() const { return _color; }
			inline void				  setColor( const Color::Rgb & p_color ) { _color = p_color; }

		  protected:
			Color::Rgb _color = Color::Rgb::randomPastel();
		};
	} // namespace Generic
} // namespace VTX
#endif
