#include "component_dialog_import.hpp"
#include "../../defines.hpp"
#include "../../lib/imgui/imfilebrowser.h"
#include "../../lib/imgui/imgui.h"

namespace VTX
{
	namespace UI
	{
		ComponentDialogImport::ComponentDialogImport( bool * const p_show ) :
			BaseComponent( p_show )
		{
		}

		void ComponentDialogImport::_draw() {}
	} // namespace UI
} // namespace VTX
