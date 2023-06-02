#ifndef __VTX_CORE_CHEMDB_CATEGORY__
#define __VTX_CORE_CHEMDB_CATEGORY__

#include <string>
#include <vector>

namespace VTX::Core::ChemDB::Category
{
	enum class TYPE : int
	{
		POLYMER,
		CARBOHYDRATE,
		LIGAND,
		ION,
		SOLVENT,
		WATER,

		UNKNOWN,
		COUNT,
	};

	inline const std::vector<std::string> TYPE_STR
		= { "Polymer", "Carbohydrate", "Ligand", "Ion", "Solvent", "Water", "Unknown" };

} // namespace VTX::Core::ChemDB::Category

#endif
