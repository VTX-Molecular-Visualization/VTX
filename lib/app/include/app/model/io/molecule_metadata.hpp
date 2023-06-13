#ifndef __VTX_APP_MODEL_IO_MOLECULE_METADATA__
#define __VTX_APP_MODEL_IO_MOLECULE_METADATA__

#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Model::IO
{
	struct MoleculeMetadata
	{
	  public:
		FilePath	path	  = FilePath();
		std::string pdbIDCode = "0000";

		std::set<uint>		  solventAtomIds					 = std::set<uint>();
		std::set<uint>		  ionAtomIds						 = std::set<uint>();
		std::set<std::string> solventResidueSymbols				 = std::set<std::string>();
		std::set<std::string> ionResidueSymbols					 = std::set<std::string>();
		bool				  isSecondaryStructureLoadedFromFile = false;
	};
} // namespace VTX::App::Model::IO
#endif
