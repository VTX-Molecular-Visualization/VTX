#include "base_selectable.hpp"
#include "action/unselect.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Selection
	{
		BaseSelectable::~BaseSelectable()
		{
			if ( isSelected() ) { VTXApp::get().getSelectionManager().unselect( this ); }
		}
	} // namespace Selection
} // namespace VTX
