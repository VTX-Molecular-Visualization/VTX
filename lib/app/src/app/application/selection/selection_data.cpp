#include "app/application/selection/selection_data.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::Selection
{
	SelectionData::SelectionData( const Component::Scene::Selectable & p_selectionComponent ) :
		_selectionComponent( &p_selectionComponent ), _valid( true )
	{
	}

	bool SelectionData::isEqualsTo( const SelectionData & p_other ) const
	{
		return _selectionComponent == p_other._selectionComponent;
	}

	std::unique_ptr<SelectionData> SelectionData::_cloneImpl() const
	{
		return std::make_unique<SelectionData>( *_selectionComponent );
	}

	const Component::Scene::Selectable & SelectionData::getSelectionComponent() const { return *_selectionComponent; }

	void SelectionData::selectAll() { _valid = true; }

	void SelectionData::set( const SelectionData & p_other ) { _valid = p_other._valid; }

	SelectionData & SelectionData::add( const SelectionData & p_other ) { return *this; }
	SelectionData & SelectionData::remove( const SelectionData & p_other )
	{
		_valid = _selectionComponent != p_other._selectionComponent;
		return *this;
	}
	SelectionData & SelectionData::intersect( const SelectionData & p_other )
	{
		_valid = _selectionComponent == p_other._selectionComponent;
		return *this;
	}
	SelectionData & SelectionData::exclude( const SelectionData & p_other )
	{
		_valid = _selectionComponent != p_other._selectionComponent;
		return *this;
	}
	bool SelectionData::contains( const SelectionData & p_other ) const
	{
		return _selectionComponent == p_other._selectionComponent;
	}

	std::string SelectionData::toString() const
	{
		Component::Scene::SceneItemComponent & sceneItem
			= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( *_selectionComponent );

		return sceneItem.getName();
	}

} // namespace VTX::App::Application::Selection
