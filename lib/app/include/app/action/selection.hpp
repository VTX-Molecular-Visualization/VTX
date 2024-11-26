#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/application/selection/selection.hpp"
#include "app/application/selection/selection_data.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/core/action/base_action.hpp"
#include <set>
#include <util/concepts.hpp>

namespace VTX::App::Action::Selection
{
	class Select final : public App::Core::Action::BaseAction
	{
	  public:
		Select(
			const Application::Selection::SelectionData & p_selectionData,
			const Application::Selection::AssignmentType  p_assignment
		)
		{
			_selectionData.emplace( &p_selectionData );
			_assignment = p_assignment;
		}

		template<ContainerOfType<const Application::Selection::SelectionData *> C1>
		Select( C1 & p_selectionData, const Application::Selection::AssignmentType p_assignment )
		{
			for ( Application::Selection::SelectionData * selectionData : p_selectionData )
			{
				_selectionData.emplace( selectionData );
			}

			_assignment = p_assignment;
		}

		void execute() override;

	  private:
		std::set<const Application::Selection::SelectionData *> _selectionData
			= std::set<const Application::Selection::SelectionData *>();

		Application::Selection::AssignmentType _assignment = Application::Selection::AssignmentType::SET;
	};

	class Unselect final : public App::Core::Action::BaseAction
	{
	  public:
		Unselect( const Application::Selection::SelectionData & p_selectionData )
		{
			_selectionData.emplace( &p_selectionData );
		}

		template<ContainerOfType<const Application::Selection::SelectionData *> C1>
		Unselect( C1 & p_selectionData )
		{
			for ( Application::Selection::SelectionData * selectionData : p_selectionData )
			{
				_selectionData.emplace( selectionData );
			}
		}

		void execute() override;

	  private:
		std::set<const Application::Selection::SelectionData *> _selectionData
			= std::set<const Application::Selection::SelectionData *>();
	};
} // namespace VTX::App::Action::Selection
#endif
