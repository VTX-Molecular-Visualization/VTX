#ifndef __VTX_APP_COMPONENT_SCENE_PICKABLE__
#define __VTX_APP_COMPONENT_SCENE_PICKABLE__

#include "app/core/ecs/base_component.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/selection/picking_info.hpp"
#include "app/selection/selection.hpp"
#include "app/selection/selection_data.hpp"
#include <functional>
#include <memory>

namespace VTX::App::Component::Scene
{
	class Pickable : public Core::ECS::BaseComponent
	{
	  public:
		enum PickType
		{
			SET,
			TOGGLE
		};

		using PickableFunc = std::function<std::unique_ptr<Selection::SelectionData>( const Selection::PickingInfo & )>;

		Pickable();
		~Pickable();

		void pick( const Selection::PickingInfo & p_ids, const PickType p_pickType ) const;
		bool isSelected( const Core::UID::uid & p_uid ) const;

		void setPickingFunction( PickableFunc p_pickableFunc ) { _pickableFunc = p_pickableFunc; };

	  private:
		bool _isSelectionDataSelected( const Selection::SelectionData & p_selectionData ) const;

		PickableFunc _pickableFunc;
	};
} // namespace VTX::App::Component::Scene
#endif
