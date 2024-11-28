#include "app/action/selection.hpp"
#include "app/application/selection/selection_manager.hpp"
#include <util/logger.hpp>

namespace VTX::App::Action::Selection
{
	void Select::execute() { CURRENT_SELECTION().selectAll( _selectionData, _assignment ); }

	void Unselect::execute()
	{
		for ( const Application::Selection::SelectionData * const selectionData : _selectionData )
		{
			Application::Selection::SelectionData & currentSelectionData
				= CURRENT_SELECTION().getSelectionData( selectionData->getSelectionComponent() );

			currentSelectionData.remove( *selectionData );

			if ( not currentSelectionData.isValid() )
			{
				CURRENT_SELECTION().unselect( selectionData->getSelectionComponent() );
			}
		}
	}
} // namespace VTX::App::Action::Selection
