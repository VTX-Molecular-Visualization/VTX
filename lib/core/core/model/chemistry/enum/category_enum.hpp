#ifndef __VTX_MODEL_CATEGORY_ENUM__
#define __VTX_MODEL_CATEGORY_ENUM__

#include <string>
#include <vector>

namespace VTX
{
	enum class CATEGORY_ENUM : int
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
	inline const std::vector<std::string> CATEGORY_ENUM_STR
		= { "Polymer", "Carbohydrate", "Ligand", "Ion", "Solvent", "Water", "Unknown" };

} // namespace VTX

#endif
