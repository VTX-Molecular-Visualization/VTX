#include "app/application/selection/selection_data.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::Selection
{
	SelectionData::SelectionData( const Component::Scene::Selectable & p_selectionComponent ) :
		_selectionComponent( &p_selectionComponent )
	{
	}

	const Component::Scene::Selectable & SelectionData::getSelectionComponent() const { return *_selectionComponent; }

	std::string SelectionData::toString() const
	{
		Component::Scene::SceneItemComponent & sceneItem
			= VTXApp::get().MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( *_selectionComponent );

		return sceneItem.getName();
	}

} // namespace VTX::App::Application::Selection
