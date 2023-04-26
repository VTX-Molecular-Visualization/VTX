#ifndef __VTX_APP_COMPONENT_IO_MOLECULE_CONFIGURATION__
#define __VTX_APP_COMPONENT_IO_MOLECULE_CONFIGURATION__

#include <set>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Component::IO
{
	struct MoleculeConfiguration
	{
		std::set<uint>		  solventAtomIds					 = std::set<uint>();
		std::set<uint>		  ionAtomIds						 = std::set<uint>();
		std::set<std::string> solventResidueSymbols				 = std::set<std::string>();
		std::set<std::string> ionResidueSymbols					 = std::set<std::string>();
		bool				  isSecondaryStructureLoadedFromFile = false;
	};

} // namespace VTX::App::Component::IO

#endif
