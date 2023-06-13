#ifndef __VTX_CORE_NEW_STRUCT_CATEGORY__
#define __VTX_CORE_NEW_STRUCT_CATEGORY__

#include "_fwd.hpp"
#include "core/chemdb/category.hpp"
#include <vector>

namespace VTX::Core::NewStruct
{
	class Category
	{
	  public:
		ChemDB::Category::TYPE category		= ChemDB::Category::TYPE::UNKNOWN;
		std::vector<size_t>	   linkedChains = std::vector<size_t>();

		Molecule * moleculePtr = nullptr;
	};

} // namespace VTX::Core::NewStruct
#endif
