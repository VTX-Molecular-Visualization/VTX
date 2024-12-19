#ifndef __VTX_APP_COMPONENT_SCENE_SELECTABLE__
#define __VTX_APP_COMPONENT_SCENE_SELECTABLE__

#include "app/core/ecs/base_component.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/selection/concepts.hpp"
#include "app/selection/selection_data.hpp"
#include <functional>
#include <memory>
#include <util/callback.hpp>

namespace VTX::App::Component::Scene
{
	class Selectable : public Core::ECS::BaseComponent
	{
	  public:
		using SelectionDataGenerator = std::function<std::unique_ptr<Selection::SelectionData>()>;

	  public:
		Selectable();
		~Selectable();

		bool isSelected() const;

		template<Selection::SelectionDataConcept T>
		void setSelectionDataGenerator()
		{
			const SelectionDataGenerator generator = [ this ]() { return std::move( std::make_unique<T>( *this ) ); };
			setSelectionDataGenerator( generator );
		}

		void setSelectionDataGenerator( const SelectionDataGenerator & p_generator );

		std::unique_ptr<Selection::SelectionData> instantiateSelectionData() const;

		Util::Callback<const Selection::SelectionData &> onSelect;
		Util::Callback<const Selection::SelectionData &> onDeselect;

	  private:
		std::unique_ptr<Selection::SelectionData> _defaultSelectionDataGenerator();

		SelectionDataGenerator _selectionDataGenerator;
	};
} // namespace VTX::App::Component::Scene
#endif
