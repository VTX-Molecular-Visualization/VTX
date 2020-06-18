#ifndef __VTX_ACTION_CHANGE_BACKGROUND_COLOR__
#define __VTX_ACTION_CHANGE_BACKGROUND_COLOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeBackgroundColor : public BaseAction
		{
		  public:
			explicit ChangeBackgroundColor( const Color::Rgb & p_color ) : _color( p_color ) {}

			virtual void execute() override { VTX_SETTING().backgroundColor = _color; };

		  private:
			const Color::Rgb _color;
		};
	} // namespace Action
} // namespace VTX
#endif
