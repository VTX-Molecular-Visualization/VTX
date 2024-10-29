#ifndef __VTX_APP_APPLICATION_SELECTION_MOLECULE_GRANULARITY__
#define __VTX_APP_APPLICATION_SELECTION_MOLECULE_GRANULARITY__

#include "app/application/selection/molecule_data.hpp"
#include "app/application/selection/selection.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include <string>
#include <vector>

namespace VTX::App::Application::Selection
{
	using namespace Component::Chemistry;

	enum class Granularity : int
	{
		ATOM = 0,
		RESIDUE,
		CHAIN,
		MOLECULE,

		COUNT
	};

	inline static std::vector<std::string> SELECTION_GRANULARITY_STR { "Atom", "Residue", "Chain", "System" };

	class MoleculeGranularity
	{
	  public:
		static MoleculeData getSelectionData( const Atom & p_atom, const Granularity & p_granularity );
		static MoleculeData getSelectionData( const Residue & p_residue, const Granularity & p_granularity );
		static MoleculeData getSelectionData( const Chain & p_chain, const Granularity & p_granularity );

		static void select( const Atom & p_atom, const Granularity & p_granularity, const AssignmentType p_assignment );
		static void unselect( const Atom & p_atom, const Granularity & p_granularity );

		static void select(
			const Residue &		 p_residue,
			const Granularity &	 p_granularity,
			const AssignmentType p_assignment
		);
		static void unselect( const Residue & p_atom, const Granularity & p_granularity );

		static void select(
			const Chain &		 p_chain,
			const Granularity &	 p_granularity,
			const AssignmentType p_assignment
		);
		static void unselect( const Chain & p_chain, const Granularity & p_granularity );
	};

} // namespace VTX::App::Application::Selection
#endif
