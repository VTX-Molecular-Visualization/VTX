#ifndef __VTX_COMPONENT_SCENE__
#define __VTX_COMPONENT_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_component_window.hpp"

namespace VTX
{
	namespace UI
	{
		class Scene : public BaseComponentWindow
		{
		  public:
			virtual const std::string & getName() const override { return ID::UI::SCENE; }
			virtual const char *		getTitle() const override { return LOCALE( "Scene.Scene" ); }
		};
	} // namespace UI
} // namespace VTX
#endif
