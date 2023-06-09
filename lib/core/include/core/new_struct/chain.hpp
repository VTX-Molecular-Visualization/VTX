#ifndef __VTX_CORE_STRUCT_CHAIN__
#define __VTX_CORE_STRUCT_CHAIN__

#include "_fwd.hpp"
#include "core/define.hpp"
#include <string>

namespace VTX::Core::Struct
{
	class Chain
	{
	  public:
		size_t	   internalIndex = INVALID_INDEX;
		Molecule * molecule;

		std::string name			  = "unknown";
		size_t		indexFirstResidue = 0;
		size_t		residueCount	  = 0;

		// std::string _originalChainID = "";
		// uint		_realResidueCount  = 0;
	};

} // namespace VTX::Core::Struct
#endif
