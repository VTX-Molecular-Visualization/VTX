#ifndef __VTX_COMPONENT_WINDOW__INSPECTOR__
#define __VTX_COMPONENT_WINDOW__INSPECTOR__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component_window.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Window
		{
			class Inspector : public BaseComponentWindow
			{
			  public:
				virtual const std::string & getName() const override { return ID::UI::INSPECTOR; }
				virtual const char *		getTitle() const override { return LOCALE( "Inspector.Inspector" ); }
			};
		} // namespace Window
	} // namespace UI
} // namespace VTX
#endif
