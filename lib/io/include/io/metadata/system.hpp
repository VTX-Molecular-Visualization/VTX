#ifndef __VTX_IO_METADATA_SYSTEM__
#define __VTX_IO_METADATA_SYSTEM__

#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::IO::Metadata
{
	struct System
	{
	  public:
		FilePath	path	  = FilePath();
		std::string pdbIDCode = "0000";

		std::set<uint>		  solventAtomIds		= std::set<uint>();
		std::set<uint>		  ionAtomIds			= std::set<uint>();
		std::set<std::string> solventResidueSymbols = std::set<std::string>();
		std::set<std::string> ionResidueSymbols		= std::set<std::string>();

		bool isSecondaryStructureLoadedFromFile = false;
	};
} // namespace VTX::IO::Metadata
#endif
