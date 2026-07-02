#ifndef __VTX_APP_ACTION_ACCESSIBILITY__
#define __VTX_APP_ACTION_ACCESSIBILITY__

#include "app/ecs.hpp"
#include "app/setting/accessibility.hpp"

namespace VTX::App::Action::Accessibility
{
	struct SetColorMode
	{
		void execute( const Setting::E_COLOR_ACCESSIBILITY_MODE p_mode )
		{
			const Entity e = ECS::getFirstEntityOnlyWithComponents<Setting::Accessibility>();
			REG().patch<Setting::Accessibility>(
				e, [ p_mode ]( Setting::Accessibility & p_setting ) { p_setting.colorMode = p_mode; }
			);
		}
	};

	struct SetForceDyslexicFont
	{
		void execute( const bool p_value )
		{
			const Entity e = ECS::getFirstEntityOnlyWithComponents<Setting::Accessibility>();
			REG().patch<Setting::Accessibility>(
				e, [ p_value ]( Setting::Accessibility & p_setting ) { p_setting.forceDyslexicFont = p_value; }
			);
		}
	};
} // namespace VTX::App::Action::Accessibility

#endif
