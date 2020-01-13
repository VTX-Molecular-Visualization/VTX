#ifndef __VTX_ACTION_CHANGE_DISPLAY_MODE__
#define __VTX_ACTION_CHANGE_DISPLAY_MODE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "../vtx_app.hpp"
#include "base_action_undonable.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionChangeDisplayMode : public BaseActionUndonable
		{
		  public:
			explicit ActionChangeDisplayMode( const Setting::UI::SYMBOL_DISPLAY_MODE p_mode ) :
				_mode( p_mode ), _modeOld( Setting::UI::symbolDisplayMode )
			{
			}

			virtual void execute() override { Setting::UI::symbolDisplayMode = _mode; };

			virtual void undo() override { Setting::UI::symbolDisplayMode = _modeOld; }

		  private:
			Setting::UI::SYMBOL_DISPLAY_MODE _mode;
			Setting::UI::SYMBOL_DISPLAY_MODE _modeOld;
		};
	} // namespace Action
} // namespace VTX
#endif
