#ifndef __VTX_APP_SELECTION_MOLECULE_GRANULARITY__
#define __VTX_APP_SELECTION_MOLECULE_GRANULARITY__

#include "app/component/chemistry/_fwd.hpp"
#include "selection.hpp"
#include "system_data.hpp"
#include <string>
#include <vector>

namespace VTX::App::Selection
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

	class SystemGranularity
	{
	  public:
		static SystemData getSelectionData( const Atom & p_atom, const Granularity & p_granularity );
		static SystemData getSelectionData( const Residue & p_residue, const Granularity & p_granularity );
		static SystemData getSelectionData( const Chain & p_chain, const Granularity & p_granularity );

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

} // namespace VTX::App::Selection
#endif
