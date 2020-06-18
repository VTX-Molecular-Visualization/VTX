#ifndef __VTX_ACTION_CHANGE_THEME__
#define __VTX_ACTION_CHANGE_THEME__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ChangeTheme : public BaseAction
		{
		  public:
			explicit ChangeTheme( const Style::THEME p_theme ) : _theme( p_theme ) {}

			virtual void execute() override
			{
				VTX_SETTING().theme = _theme;
				VTXApp::get().getUI().setTheme();
			};

		  private:
			const Style::THEME _theme;
		};
	} // namespace Action
} // namespace VTX
#endif
