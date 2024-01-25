#ifndef __VTX_APP_COMPONENT_SCENE_SELECTABLE__
#define __VTX_APP_COMPONENT_SCENE_SELECTABLE__

#include "app/application/ecs/component_registration.hpp"
#include "app/application/selection/concepts.hpp"
#include "app/application/selection/selection_data.hpp"
#include "app/core/ecs/base_component.hpp"
#include <functional>
#include <memory>

namespace VTX::App::Component::Scene
{
	class Selectable : public Core::ECS::BaseComponent
	{
	  private:
		inline static Application::ECS::Registration<Selectable> registration { "Scene::SelectableComponent" };

	  public:
		using SelectionDataGenerator = std::function<std::unique_ptr<Application::Selection::SelectionData>()>;

	  public:
		Selectable();
		~Selectable();

		bool isSelected() const;

		template<Application::Selection::SelectionDataConcept T>
		void setSelectionDataGenerator()
		{
			const SelectionDataGenerator generator = [ this ]() { return std::move( std::make_unique<T>( *this ) ); };
			setSelectionDataGenerator( generator );
		}

		void setSelectionDataGenerator( const SelectionDataGenerator & p_generator );

		std::unique_ptr<Application::Selection::SelectionData> instantiateSelectionData() const;

	  private:
		SelectionDataGenerator _selectionDataGenerator;

		std::unique_ptr<Application::Selection::SelectionData> _defaultSelectionDataGenerator();
	};
} // namespace VTX::App::Component::Scene
#endif
