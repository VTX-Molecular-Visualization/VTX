#ifndef __VTX_CORE_CHEMDB_SECONDARY_STRUCTURE__
#define __VTX_CORE_CHEMDB_SECONDARY_STRUCTURE__

#include <util/constants.hpp>

namespace VTX::Core::ChemDB::SecondaryStructure
{
	enum class TYPE : uchar
	{
		HELIX_ALPHA_RIGHT = 0,
		HELIX_ALPHA_LEFT  = 1,
		HELIX_3_10_RIGHT  = 2,
		HELIX_3_10_LEFT	  = 3,
		HELIX_PI		  = 4,
		STRAND			  = 5,
		TURN			  = 6,
		COIL			  = 7,
		UNKNOWN,
		COUNT
	};

	const std::string_view SYMBOL_NAME[ (int)TYPE::COUNT ] = { "Alpha Helix Right",
															   "Alpha Helix Left",
															   "3-10 Helix Right",
															   "3-10 Helix Left",
															   "Pi Helix",
															   "Strand",
															   "Turn",
															   "Coil",
															   "Unknown" };

	TYPE		pdbFormattedToEnum( const std::string & p_str );
	std::string enumToPdbFormatted( const TYPE p_enum );

} // namespace VTX::Core::ChemDB::SecondaryStructure

#endif
