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
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseColorable, T>::value>>
		class ColorableChangeColor : public BaseAction
		{
		  public:
			explicit ColorableChangeColor( T & p_colorable, const Vec3f & p_color ) :
				_colorable( p_colorable ), _color( p_color )
			{
			}

			virtual void execute() override { _colorable.setColor( _color ); }

		  private:
			Generic::BaseColorable & _colorable;
			const Vec3f				 _color;
		};
	} // namespace Action
} // namespace VTX
#endif
