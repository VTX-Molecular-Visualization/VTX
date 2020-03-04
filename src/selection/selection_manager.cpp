#include "selection_manager.hpp"

namespace VTX
{
	namespace Selection
	{
		void SelectionManager::select( BaseSelectable * const p_selectable )
		{
			_selected.emplace( p_selectable );
			p_selectable->setSelected( true );
		}

		void SelectionManager::unselect( BaseSelectable * const p_selectable )
		{
			_selected.erase( p_selectable );
			p_selectable->setSelected( false );
		}
	} // namespace Selection
} // namespace VTX
