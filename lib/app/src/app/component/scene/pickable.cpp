#include "app/component/scene/pickable.hpp"
#include "app/action/selection.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/application/system/action_manager.hpp"

namespace VTX::App::Component::Scene
{
	Pickable::Pickable() {}
	Pickable::~Pickable() {}

	void Pickable::pick( const Application::Selection::PickingInfo & p_ids, const PickType p_pickType ) const
	{
		const std::unique_ptr<Application::Selection::SelectionData> selectionData = _pickableFunc( p_ids );

		switch ( p_pickType )
		{
		case PickType::SET:
		{
			VTX_ACTION().execute<Action::Selection::Select>(
				*selectionData, Application::Selection::AssignmentType::SET
			);
		}
		break;

		case PickType::TOGGLE:
		{
			if ( _isSelectionDataSelected( *selectionData ) )
			{
				VTX_ACTION().execute<Action::Selection::Unselect>( *selectionData );
			}
			else
			{
				VTX_ACTION().execute<Action::Selection::Select>(
					*selectionData, Application::Selection::AssignmentType::APPEND
				);
			}
		}
		break;

		default: assert( false ); break;
		}
	}
	bool Pickable::isSelected( const Core::UID::uid & p_uid ) const
	{
		const std::unique_ptr<Application::Selection::SelectionData> selectionData
			= _pickableFunc( { p_uid, Core::UID::INVALID_UID } );

		return _isSelectionDataSelected( *selectionData );
	}

	bool Pickable::_isSelectionDataSelected( const Application::Selection::SelectionData & p_selectionData ) const
	{
		return CURRENT_SELECTION().isSelected( p_selectionData.getSelectionComponent() )
			   && CURRENT_SELECTION()
					  .getSelectionData( p_selectionData.getSelectionComponent() )
					  .contains( p_selectionData );
	}

} // namespace VTX::App::Component::Scene
