#ifndef __VTX_APP_COMPONENT_SCENE_PICKABLE__
#define __VTX_APP_COMPONENT_SCENE_PICKABLE__

#include "app/application/ecs/component_registration.hpp"
#include "app/application/selection/picking_info.hpp"
#include "app/application/selection/selection.hpp"
#include "app/application/selection/selection_data.hpp"
#include "app/core/ecs/base_component.hpp"
#include <functional>
#include <memory>

namespace VTX::App::Component::Scene
{
	class Pickable : public Core::ECS::BaseComponent
	{
	  public:
		using PickableFunc = std::function<
			std::unique_ptr<Application::Selection::SelectionData>( const Application::Selection::PickingInfo & )>;

	  private:
		inline static Application::ECS::Registration<Pickable> registration { "Scene::PickableComponent" };

	  public:
		Pickable();
		~Pickable();

		void pick(
			const Application::Selection::PickingInfo &	   p_ids,
			const Application::Selection::AssignmentType & p_assignmentType
		) const;
		bool isSelected( const Core::UID::uid & p_uid ) const;

		void setPickingFunction( PickableFunc p_pickableFunc ) { _pickableFunc = p_pickableFunc; };

	  private:
		PickableFunc _pickableFunc;
	};
} // namespace VTX::App::Component::Scene
#endif
