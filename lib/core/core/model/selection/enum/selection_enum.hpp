#ifndef __VTX_SELECTION_ENUM__
#define __VTX_SELECTION_ENUM__

#include <string>
#include <vector>

namespace VTX::Selection
{
	enum class Granularity : int
	{
		ATOM = 0,
		RESIDUE,
		CHAIN,
		MOLECULE,

		COUNT
	};

	inline static std::vector<std::string> SELECTION_GRANULARITY_STR { "Atom", "Residue", "Chain", "Molecule" };

} // namespace VTX::Selection
#endif
