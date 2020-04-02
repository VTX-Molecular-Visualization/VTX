#include "base_selectable.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Selection
	{
		BaseSelectable::~BaseSelectable()
		{
			// Don't use action because we need to unselect now.
			if ( isSelected() ) { VTXApp::get().getSelectionManager().unselect( this ); }
		}
	} // namespace Selection
} // namespace VTX
