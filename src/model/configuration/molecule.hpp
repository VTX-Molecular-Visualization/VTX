#ifndef __VTX_MODEL_CONFIGURATION_MOLECULE__
#define __VTX_MODEL_CONFIGURATION_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
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

				int sceneIndex = 0;
			};

		} // namespace Configuration
	}	  // namespace Model
} // namespace VTX

#endif
