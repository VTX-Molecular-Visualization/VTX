#include "app/action/selection.hpp"
#include "app/selection/selection_manager.hpp"
#include <util/logger.hpp>

namespace VTX::App::Action::Selection
{
	void Select::execute() { CURRENT_SELECTION().selectAll( _selectionData, _assignment ); }

	void Unselect::execute()
	{
		for ( const App::Selection::SelectionData * const selectionData : _selectionData )
		{
			App::Selection::SelectionData & currentSelectionData
				= CURRENT_SELECTION().getSelectionData( selectionData->getSelectionComponent() );

			currentSelectionData.remove( *selectionData );

			if ( not currentSelectionData.isValid() )
			{
				CURRENT_SELECTION().unselect( selectionData->getSelectionComponent() );
			}
		}
	}
} // namespace VTX::App::Action::Selection
