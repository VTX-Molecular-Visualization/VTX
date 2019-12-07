#include "settings.hpp"
#include "renderer/base_renderer.hpp"
#include "ui/user_interface.hpp"

namespace VTX
{
	namespace Setting
	{
		namespace UI
		{
			VTX::UI::THEME		theme			  = VTX::UI::THEME::LIGHT;
			SYMBOL_DISPLAY_MODE symbolDisplayMode = SYMBOL_DISPLAY_MODE::SHORT;
		}; // namespace UI

		namespace Rendering
		{
			Renderer::MODE	  mode	  = Renderer::MODE::DEFERRED;
			Renderer::SHADING shading = Renderer::SHADING::LAMBERT;
			bool			  useAA	  = true;
			bool			  useSSAO = true;
		}; // namespace Rendering
	}	   // namespace Setting
} // namespace VTX
