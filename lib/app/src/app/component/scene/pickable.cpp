#include "app/component/scene/pickable.hpp"
#include "app/action/selection.hpp"
#include "app/selection/selection_manager.hpp"
#include "app/core/action/action_system.hpp"

namespace VTX::App::Component::Scene
{
	Pickable::Pickable() {}
	Pickable::~Pickable() {}

	void Pickable::pick( const Selection::PickingInfo & p_ids, const PickType p_pickType ) const
	{
		const std::unique_ptr<Selection::SelectionData> selectionData = _pickableFunc( p_ids );

		switch ( p_pickType )
		{
		case PickType::SET:
		{
			ACTION_SYSTEM().execute<Action::Selection::Select>(
				*selectionData, Selection::AssignmentType::SET
			);
		}
		break;

		case PickType::TOGGLE:
		{
			bool isSelected = _isSelectionDataSelected( *selectionData );

			if ( _isSelectionDataSelected( *selectionData ) )
			{
				ACTION_SYSTEM().execute<Action::Selection::Unselect>( *selectionData );
			}
			else
			{
				ACTION_SYSTEM().execute<Action::Selection::Select>(
					*selectionData, Selection::AssignmentType::APPEND
				);
			}
		}
		break;

		default: assert( false ); break;
		}
	}
	bool Pickable::isSelected( const Core::UID::uid & p_uid ) const
	{
		const std::unique_ptr<Selection::SelectionData> selectionData
			= _pickableFunc( { p_uid, Core::UID::INVALID_UID } );

		return _isSelectionDataSelected( *selectionData );
	}

	bool Pickable::_isSelectionDataSelected( const Selection::SelectionData & p_selectionData ) const
	{
		return CURRENT_SELECTION().isSelected( p_selectionData.getSelectionComponent() )
			   && CURRENT_SELECTION()
					  .getSelectionData( p_selectionData.getSelectionComponent() )
					  .contains( p_selectionData );
	}

} // namespace VTX::App::Component::Scene
