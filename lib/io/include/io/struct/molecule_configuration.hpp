#ifndef __VTX_IO_STRUCT_MOLECULE_CONFIGURATION__
#define __VTX_IO_STRUCT_MOLECULE_CONFIGURATION__

#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::IO::Struct
{
	struct MoleculeConfiguration
	{
	  public:
		std::set<uint>		  solventAtomIds		= std::set<uint>();
		std::set<uint>		  ionAtomIds			= std::set<uint>();
		std::set<std::string> solventResidueSymbols = std::set<std::string>();
		std::set<std::string> ionResidueSymbols		= std::set<std::string>();
	};
} // namespace VTX::IO::Struct
#endif
