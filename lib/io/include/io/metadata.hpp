#ifndef __VTX_IO_METADATA__
#define __VTX_IO_METADATA__

#include "io/constants.hpp"
#include <string>

namespace VTX::IO
{
	/**
	 * @brief Store metadata about the system.
	 */
	struct Metadata
	{
		FilePath	   path;
		std::string	   pdbIDCode	 = std::string( PDB_ID_CODE_DEFAULT );
		std::string	   name			 = "";
		READER_OPTION  readerOption	 = READER_OPTION::ALL;
		TOPOLOGY_STATE topologyState = TOPOLOGY_STATE::OK;

		/*
		std::set<uint>		  solventAtomIds		= std::set<uint>();
		std::set<uint>		  ionAtomIds			= std::set<uint>();
		std::set<std::string> solventResidueSymbols = std::set<std::string>();
		std::set<std::string> ionResidueSymbols		= std::set<std::string>();
		*/
	};
} // namespace VTX::IO
#endif
