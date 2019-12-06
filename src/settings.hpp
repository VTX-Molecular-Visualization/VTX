#ifndef __VTX_SETTINGS__
#define __VTX_SETTINGS__

#ifdef _MSC_VER
#pragma once
#endif

#include "renderer/base_renderer.hpp"
#include "ui/user_interface.hpp"

namespace VTX
{
	namespace Setting
	{
		namespace UI
		{
			extern VTX::UI::THEME theme;
		};

		namespace Rendering
		{
			extern Renderer::MODE	 mode;
			extern Renderer::SHADING shading;
			extern bool				 useAA;
			extern bool				 useSSAO;
		}; // namespace Rendering
	}	   // namespace Setting
} // namespace VTX
#endif
