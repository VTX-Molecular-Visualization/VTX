#ifndef __VTX_MODEL_CONFIGURATION_MOLECULE__
#define __VTX_MODEL_CONFIGURATION_MOLECULE__

#include "model/define.hpp"
#include <set>

namespace VTX
{
	namespace Model
	{
		namespace Configuration
		{
			struct Molecule
			{
				std::set<uint>		  solventAtomIds					 = std::set<uint>();
				std::set<uint>		  ionAtomIds						 = std::set<uint>();
				std::set<std::string> solventResidueSymbols				 = std::set<std::string>();
				std::set<std::string> ionResidueSymbols					 = std::set<std::string>();
				bool				  isSecondaryStructureLoadedFromFile = false;
			};

		} // namespace Configuration
	}	  // namespace Model
} // namespace VTX

#endif
