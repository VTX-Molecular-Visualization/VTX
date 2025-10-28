#ifndef __VTX_APP_COMPONENT_SYSTEM_METADATA__
#define __VTX_APP_COMPONENT_SYSTEM_METADATA__

#include <set>
#include <util/types.hpp>

namespace VTX::App::Component
{
	class SystemMetadata
	{
	  public:
		FilePath	path;
		std::string pdbIDCode;
		bool		structureHasChanged				   = false;
		bool		isSecondaryStructureLoadedFromFile = false;

		// TODO: keep that?
		std::set<uint>		  solventAtomIds;
		std::set<uint>		  ionAtomIds;
		std::set<std::string> solventResidueSymbols;
		std::set<std::string> ionResidueSymbols;
	};
} // namespace VTX::App::Component
#endif
