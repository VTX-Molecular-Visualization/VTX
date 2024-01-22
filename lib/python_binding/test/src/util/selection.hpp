#ifndef __VTX_APP_TEST_UTIL_SELECTION__
#define __VTX_APP_TEST_UTIL_SELECTION__

#include <app/application/selection/molecule_data.hpp>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_data.hpp>
#include <string>
#include <util/math/range_list.hpp>

namespace VTX::App::Test::Util
{
	class Selection
	{
		using MoleculeData	= VTX::App::Application::Selection::MoleculeData;
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
		static SelectionObj createSelection( const std::vector<SelectionData *> & p_items );

		static std::unique_ptr<MoleculeData> generateMoleculeData(
			const std::string &			p_molName,
			const std::vector<size_t> & p_chains   = {},
			const std::vector<size_t> & p_residues = {},
			const std::vector<size_t> & p_atoms	   = {}
		);
	};
} // namespace VTX::App::Test::Util
#endif