#include "selection_manager.hpp"
#include "vtx_app.hpp"
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
				VTX_EVENT( new Event::VTXEvent( Event::Global::SELECTION_CHANGE ) );
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
				VTX_EVENT( new Event::VTXEvent( Event::Global::SELECTION_CHANGE ) );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Item not selected" );
				VTX_ERROR( p_e.what() );
			}
		}

		void SelectionManager::clear()
		{
			for ( auto it = _selected.begin(); it != _selected.end(); it++ )
				( *it )->setSelected( false );

			_selected.clear();
		}
	} // namespace Selection
} // namespace VTX
