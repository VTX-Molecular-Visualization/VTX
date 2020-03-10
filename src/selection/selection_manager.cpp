#include "selection_manager.hpp"
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
				p_selectable->setSelected( true );
				VTX_DEBUG(std::to_string(_selected.size()));
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
				p_selectable->setSelected( false );
				VTX_DEBUG( std::to_string( _selected.size() ) );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Item not selected" );
				VTX_ERROR( p_e.what() );
			}
		}
	} // namespace Selection
} // namespace VTX
