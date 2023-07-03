#ifndef __VTX_IO_METADATA_MOLECULE__
#define __VTX_IO_METADATA_MOLECULE__

#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::IO::Metadata
{
	struct Molecule
	{
	  public:
		FilePath	path	  = FilePath();
		std::string pdbIDCode = "0000";

		std::string name = "";

		std::set<uint>		  solventAtomIds					 = std::set<uint>();
		std::set<uint>		  ionAtomIds						 = std::set<uint>();
		std::set<std::string> solventResidueSymbols				 = std::set<std::string>();
		std::set<std::string> ionResidueSymbols					 = std::set<std::string>();
		bool				  isSecondaryStructureLoadedFromFile = false;
	};
} // namespace VTX::IO::Metadata
#endif
