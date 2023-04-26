#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ENUM_CATEGORY__
#define __VTX_APP_COMPONENT_CHEMISTRY_ENUM_CATEGORY__

#include <string>
#include <vector>

namespace VTX::App::Component::Chemistry
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

} // namespace VTX::App::Component::Chemistry

#endif
