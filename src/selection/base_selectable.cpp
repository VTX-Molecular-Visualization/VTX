#include "base_selectable.hpp"
#include "action/unselect.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Selection
	{
		BaseSelectable::~BaseSelectable()
		{
			if ( isSelected() )
			{
				VTX_DEBUG("~BaseSelectable");
				VTXApp::get().getActionManager().execute( new Action::Unselect( *this ) );
			}
		}
	} // namespace Selection
} // namespace VTX
