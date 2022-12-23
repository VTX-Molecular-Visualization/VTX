#ifndef __VTX_UI_TOOL_DEFAULT_RENDER_WINDOW__
#define __VTX_UI_TOOL_DEFAULT_RENDER_WINDOW__

#include "__new_archi/ui/core/tool_registration.hpp"
#include "keys.hpp"

namespace VTX::UI::DefaultTools
{
	class RenderWindow : public VTX::UI::Core::BaseVTXUITool
	{
		REGISTER_TOOL( RenderWindow, RENDER_WINDOW_KEY );

	  public:
		RenderWindow();

		void instantiateTool() override;
	};
} // namespace VTX::UI::DefaultTools

#endif
