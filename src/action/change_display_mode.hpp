#ifndef __VTX_ACTION_CHANGE_DISPLAY_MODE__
#define __VTX_ACTION_CHANGE_DISPLAY_MODE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeDisplayMode : public BaseAction
		{
		  public:
			explicit ChangeDisplayMode( const Style::SYMBOL_DISPLAY_MODE p_mode ) : _mode( p_mode ) {}

			virtual void execute() override { VTX_SETTING().symbolDisplayMode = _mode; };

		  private:
			const Style::SYMBOL_DISPLAY_MODE _mode;
		};
	} // namespace Action
} // namespace VTX
#endif
