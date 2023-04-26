#ifndef __VTX_APP_INTERNAL_CHEMDB_UNKNOWN_RESIDUE_DATA__
#define __VTX_APP_INTERNAL_CHEMDB_UNKNOWN_RESIDUE_DATA__

#include <string>

namespace VTX::App::Internal::ChemDB
{
	struct UnknownResidueData
	{
	  public:
		std::string symbolStr;
		std::string symbolName;
	};
} // namespace VTX::App::Internal::ChemDB

#endif
