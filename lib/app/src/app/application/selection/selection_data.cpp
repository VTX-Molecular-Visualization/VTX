#include "app/application/selection/selection_data.hpp"
#include "app/component/scene/selectable.hpp"

namespace VTX::App::Application::Selection
{
	SelectionData::SelectionData( const Component::Scene::Selectable & p_selectionComponent ) :
		_selectionComponent( &p_selectionComponent )
	{
	}

	const Component::Scene::Selectable & SelectionData::getSelectionComponent() const { return *_selectionComponent; }
} // namespace VTX::App::Application::Selection
