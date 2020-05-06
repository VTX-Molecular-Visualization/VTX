#ifndef __VTX_ACTION_COLORABLE_CHANGE_COLOR__
#define __VTX_ACTION_COLORABLE_CHANGE_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_colorable.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ColorableChangeColor : public BaseAction
		{
		  public:
			explicit ColorableChangeColor( Generic::BaseColorable & p_colorable, const Color::Rgb & p_color ) :
				_colorable( p_colorable ), _color( p_color )
			{
			}

			virtual void execute() override { _colorable.setColor( _color ); }

		  private:
			Generic::BaseColorable & _colorable;
			const Color::Rgb		 _color;
		};
	} // namespace Action
} // namespace VTX
#endif
