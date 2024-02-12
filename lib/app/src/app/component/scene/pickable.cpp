#include "app/component/scene/pickable.hpp"
#include "app/action/selection.hpp"
#include "app/application/action/action_manager.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Scene
{
	Pickable::Pickable() {}
	Pickable::~Pickable() {}

	void Pickable::pick(
		const Application::Selection::PickingInfo &	   p_ids,
		const Application::Selection::AssignmentType & p_assignmentType
	) const
	{
		const std::unique_ptr<Application::Selection::SelectionData> selectionData = _pickableFunc( p_ids );
		VTX_ACTION().execute<Action::Selection::Select>( *selectionData, p_assignmentType );
	}
	bool Pickable::isSelected( const Core::UID::uid & p_uid ) const
	{
		const std::unique_ptr<Application::Selection::SelectionData> selectionData
			= _pickableFunc( { p_uid, Core::UID::INVALID_UID } );

		return CURRENT_SELECTION().isSelected( selectionData->getSelectionComponent() )
			   && CURRENT_SELECTION()
					  .getSelectionData( selectionData->getSelectionComponent() )
					  .contains( *selectionData );
	}

} // namespace VTX::App::Component::Scene
