#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/component/scene/selectable.hpp"
#include "app/core/action/base_action.hpp"
#include "app/selection/selection.hpp"
#include "app/selection/selection_data.hpp"
#include <set>
#include <util/concepts.hpp>

namespace VTX::App::Action::Selection
{
	class Select final : public App::Core::Action::BaseAction
	{
	  public:
		Select(
			const App::Selection::SelectionData & p_selectionData,
			const App::Selection::AssignmentType  p_assignment
		)
		{
			_selectionData.emplace( &p_selectionData );
			_assignment = p_assignment;
		}

		template<ContainerOfType<const App::Selection::SelectionData *> C1>
		Select( C1 & p_selectionData, const App::Selection::AssignmentType p_assignment )
		{
			for ( App::Selection::SelectionData * selectionData : p_selectionData )
			{
				_selectionData.emplace( selectionData );
			}

			_assignment = p_assignment;
		}

		void execute() override;

	  private:
		std::set<const App::Selection::SelectionData *> _selectionData
			= std::set<const App::Selection::SelectionData *>();

		App::Selection::AssignmentType _assignment = App::Selection::AssignmentType::SET;
	};

	class Unselect final : public App::Core::Action::BaseAction
	{
	  public:
		Unselect( const App::Selection::SelectionData & p_selectionData )
		{
			_selectionData.emplace( &p_selectionData );
		}

		template<ContainerOfType<const App::Selection::SelectionData *> C1>
		Unselect( C1 & p_selectionData )
		{
			for ( App::Selection::SelectionData * selectionData : p_selectionData )
			{
				_selectionData.emplace( selectionData );
			}
		}

		void execute() override;

	  private:
		std::set<const App::Selection::SelectionData *> _selectionData
			= std::set<const App::Selection::SelectionData *>();
	};
} // namespace VTX::App::Action::Selection
#endif
