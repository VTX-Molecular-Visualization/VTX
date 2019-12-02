#ifndef __VTX_SETTINGS__
#define __VTX_SETTINGS__

#include "renderer/base_renderer.hpp"
#include "ui/user_interface.hpp"

namespace VTX
{
	namespace Setting
	{
		namespace UI
		{
			VTX::UI::THEME theme = VTX::UI::THEME::LIGHT;
		};

		namespace Rendering
		{
			Renderer::MODE	  mode	  = Renderer::MODE::DEFERRED;
			Renderer::SHADING shading = Renderer::SHADING::LAMBERT;
			bool			  useAA	  = true;
			bool			  useSSAO = true;
			bool			  useBlur = true;
		}; // namespace Rendering
	}	   // namespace Setting
} // namespace VTX
#endif
