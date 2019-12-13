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
			View::MOLECULE_REPRESENTATION representation = View::MOLECULE_REPRESENTATION::BALL_AND_STICK;
			View::MOLECULE_COLOR_MODE	  colorMode		 = View::MOLECULE_COLOR_MODE::ATOM;
			Renderer::MODE				  mode			 = Renderer::MODE::DEFERRED;
			Renderer::SHADING			  shading		 = Renderer::SHADING::LAMBERT;
			bool						  useAA			 = false;
			bool						  useSSAO		 = false;
		}; // namespace Rendering
	}	   // namespace Setting
} // namespace VTX
