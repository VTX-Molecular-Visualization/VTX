#ifndef __VTX_CORE_CHEMDB_SECONDARY_STRUCTURE__
#define __VTX_CORE_CHEMDB_SECONDARY_STRUCTURE__

#include <array>
#include <util/constants.hpp>

namespace VTX::Core::ChemDB::SecondaryStructure
{
	enum class TYPE : uint8_t
	{
		UNKNOWN,
		HELIX_ALPHA_RIGHT,
		HELIX_ALPHA_LEFT,
		HELIX_3_10_RIGHT,
		HELIX_3_10_LEFT,
		HELIX_PI,
		STRAND,
		TURN,
		COIL,
		COUNT
	};

	constexpr std::array<std::string_view, size_t( TYPE::COUNT )> SYMBOL_NAME = { "Unknown",
																				  "Alpha Helix Right",
																				  "Alpha Helix Left",
																				  "3-10 Helix Right",
																				  "3-10 Helix Left",
																				  "Pi Helix",
																				  "Strand",
																				  "Turn",
																				  "Coil" };

	TYPE		pdbFormattedToEnum( const std::string & p_str );
	std::string enumToPdbFormatted( const TYPE p_enum );

} // namespace VTX::Core::ChemDB::SecondaryStructure

#endif
