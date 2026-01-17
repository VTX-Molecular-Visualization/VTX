#ifndef __VTX_CORE_CHEMDB_CATEGORY__
#define __VTX_CORE_CHEMDB_CATEGORY__

#include <array>
#include <string>

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

	constexpr std::array<std::string_view, size_t( TYPE::COUNT )> TYPE_STR
		= { "Polymer", "Carbohydrate", "Ligand", "Ion", "Solvent", "Water", "Unknown" };

} // namespace VTX::Core::ChemDB::Category

#endif
