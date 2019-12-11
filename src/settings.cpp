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
			View::MOLECULE_REPRESENTATION reprensation = View::MOLECULE_REPRESENTATION::VAN_DER_WAALS;
			Renderer::MODE				  mode		   = Renderer::MODE::DEFERRED;
			Renderer::SHADING			  shading	   = Renderer::SHADING::LAMBERT;
			bool						  useAA		   = false;
			bool						  useSSAO	   = false;
		}; // namespace Rendering
	}	   // namespace Setting
} // namespace VTX
