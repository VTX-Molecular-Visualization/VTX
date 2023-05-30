#ifndef __VTX_APP_INTERNAL_CHEMDB_COLOR__
#define __VTX_APP_INTERNAL_CHEMDB_COLOR__

#include <string>
#include <util/color/rgba.hpp>
#include <vector>

namespace VTX::Core::ChemDB::Color
{
	enum class COLOR_MODE : int
	{
		ATOM_CHAIN,
		ATOM_PROTEIN,
		ATOM_CUSTOM,
		RESIDUE,
		CHAIN,
		PROTEIN,
		CUSTOM,
		INHERITED
	};
	static const std::vector<std::string> COLOR_MODE_STRING { "Atom-Chain", "Atom-Object", "Atom-Custom", "Residue",
															  "Chain",		"Molecule",	   "Custom",	  "Inherited" };
	enum class COLOR_BLENDING_MODE : int
	{
		HARD,
		SOFT
	};
	inline static const std::vector<std::string> COLOR_BLENDING_MODE_STRING { "Hard", "Soft" };

	enum class SECONDARY_STRUCTURE_COLOR_MODE : int
	{
		JMOL,
		PROTEIN,
		CHAIN,
		RESIDUE,
		CUSTOM
	};
	inline static const std::vector<std::string> SECONDARY_STRUCTURE_COLOR_MODE_STRING { "JMOL",
																						 "Molecule",
																						 "Chain",
																						 "Residue",
																						 "Custom" };
} // namespace VTX::Core::ChemDB::Color

#endif
