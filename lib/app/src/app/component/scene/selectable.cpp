#include "app/component/scene/selectable.hpp"
#include "app/application/selection/selection.hpp"
#include "app/application/selection/selection_manager.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Scene
{
	Selectable::Selectable()
	{
		_selectionDataGenerator = [ this ]() { return this->_defaultSelectionDataGenerator(); };
	}
	Selectable::~Selectable() {}

	bool Selectable::isSelected() const { return CURRENT_SELECTION().isSelected( *this ); }

	void Selectable::setSelectionDataGenerator( const SelectionDataGenerator & p_generator )
	{
		_selectionDataGenerator = p_generator;
	}

	std::unique_ptr<Application::Selection::SelectionData> Selectable::instantiateSelectionData() const
	{
		return _selectionDataGenerator();
	}

	std::unique_ptr<Application::Selection::SelectionData> Selectable::_defaultSelectionDataGenerator()
	{
		return std::move( std::make_unique<Application::Selection::SelectionData>( *this ) );
	}

} // namespace VTX::App::Component::Scene
