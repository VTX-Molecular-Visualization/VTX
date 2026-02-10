#ifndef __VTX_APP_SYSTEM_METADATA__
#define __VTX_APP_SYSTEM_METADATA__

#include <set>
#include <util/types.hpp>

namespace VTX::App::System
{
	/**
	 * @brief Store metadata about the system.
	 */
	struct Metadata
	{
		FilePath	path;
		std::string pdbIDCode;
		std::string name;
		bool		structureHasChanged				   = false;
		bool		isSecondaryStructureLoadedFromFile = false;

		// TODO: keep that?
		std::set<uint>		  solventAtomIds;
		std::set<uint>		  ionAtomIds;
		std::set<std::string> solventResidueSymbols;
		std::set<std::string> ionResidueSymbols;
	};
} // namespace VTX::App::System
#endif
