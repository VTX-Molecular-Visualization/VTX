#ifndef __VTX_APP_TEST_UTIL_SELECTION__
#define __VTX_APP_TEST_UTIL_SELECTION__

#include <app/application/selection/concepts.hpp>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_data.hpp>
#include <app/application/selection/system_data.hpp>
#include <string>
#include <util/concepts.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::Test::Util
{
	class Selection
	{
		using SystemData	= VTX::App::Application::Selection::SystemData;
		using SelectionData = VTX::App::Application::Selection::SelectionData;
		using SelectionObj	= VTX::App::Application::Selection::Selection;

	  public:
		static bool checkSelection(
			const std::string &	 p_selName,
			const std::string &	 p_command,
			const SelectionObj & p_expectedResult
		);

		static SelectionObj createSelection( const SelectionData & p_item );
		static SelectionObj createSelection( const std::unique_ptr<SelectionData> & p_itemPtr );

		template<Application::Selection::SelectionDataConcept T>
		static SelectionObj createSelection( const std::initializer_list<T> & p_items )
		{
			SelectionObj res = SelectionObj();

			for ( const T & sourceItemPtr : p_items )
			{
				res.select(
					sourceItemPtr.getSelectionComponent(),
					sourceItemPtr,
					VTX::App::Application::Selection::AssignmentType::APPEND
				);
			}

			return res;
		}

		template<ContainerOfType<const SelectionData *> C>
		static SelectionObj createSelection( const C & p_items )
		{
			SelectionObj res = SelectionObj();

			for ( const SelectionData * const sourceItemPtr : p_items )
			{
				res.select(
					sourceItemPtr->getSelectionComponent(),
					*sourceItemPtr,
					VTX::App::Application::Selection::AssignmentType::APPEND
				);
			}

			return res;
		}

		static std::unique_ptr<SystemData> generateSystemData(
			const std::string &				  p_molName,
			const std::vector<size_t> &		  p_chains	 = {},
			const std::vector<size_t> &		  p_residues = {},
			const std::vector<atom_index_t> & p_atoms	 = {}
		);
	};
} // namespace VTX::PythonFixture::Test::Util
#endif
