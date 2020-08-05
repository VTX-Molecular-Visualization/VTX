#include "selection_manager.hpp"
#include "util/logger.hpp"
#include <string>

namespace VTX
{
	namespace Selection
	{
		void SelectionManager::select( BaseSelectable * const p_selectable )
		{
			try
			{
				_selected.emplace( p_selectable );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Item already selected" );
				VTX_ERROR( p_e.what() );
			}
		}

		void SelectionManager::unselect( BaseSelectable * const p_selectable )
		{
			try
			{
				_selected.erase( p_selectable );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Item not selected" );
				VTX_ERROR( p_e.what() );
			}
		}
	} // namespace Selection
} // namespace VTX
