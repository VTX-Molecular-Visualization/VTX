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
		std::string name;
		std::string pdbIDCode;
		bool		isSecondaryStructureLoadedFromFile = false;
		bool		isTopologyDegenerated			   = false;
	};
} // namespace VTX::App::System
#endif
