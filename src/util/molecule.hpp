#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

#include "util/filesystem.hpp"
#include <string>

namespace VTX::Util::Molecule
{
	inline static std::string hetatmDictionary = "";

	static void loadHetatmDictionary()
	{
		Util::Filesystem::readPath( Util::Filesystem::HET_DICTIONARY_PATH, hetatmDictionary );
	}
	static void unloadHetatmDictionary() { hetatmDictionary = ""; }
	static bool isHetatmDictionaryLoaded() { return hetatmDictionary != ""; }

	static std::string getResidueFullName( const std::string & p_residueSymbol )
	{
		const bool dicoWasLoad = hetatmDictionary != "";

		if ( !dicoWasLoad )
			loadHetatmDictionary();

		const std::string keyName	 = "HETNAM     " + p_residueSymbol + " ";
		const std::string keyNameEnd = "FORMUL      " + p_residueSymbol + " ";

		const size_t indexStartName = hetatmDictionary.find( keyName );
		const size_t indexEndName	= hetatmDictionary.find( keyNameEnd, indexStartName );

		const std::string namePart = hetatmDictionary.substr( indexStartName, indexEndName - indexStartName );

		std::string name;

		size_t lastCheckedPos = 0;
		size_t endOfLinePos	  = namePart.find( '\n' );

		while ( endOfLinePos != std::string::npos )
		{
			name += namePart.substr( lastCheckedPos + 15, ( endOfLinePos - lastCheckedPos ) - 15 );
			lastCheckedPos = endOfLinePos + 1;
			endOfLinePos   = namePart.find( '\n', lastCheckedPos );
		}

		if ( !dicoWasLoad )
			unloadHetatmDictionary();

		return name;
	}

} // namespace VTX::Util::Molecule

#endif
