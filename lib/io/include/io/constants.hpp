#ifndef __VTX_IO_CONSTANTS__
#define __VTX_IO_CONSTANTS__

#include <util/types.hpp>

namespace VTX::IO
{
	/**
	 * @brief Default PDB ID code.
	 */
	constexpr std::string_view PDB_ID_CODE_DEFAULT = "----";

	/**
	 * @brief Extra computation when missing informations.
	 */
	enum struct READER_OPTION : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		NONE								= 0,
		RECOMPUTE_MISSING_BONDS				= 1 << 0,
		GUESS_UNKNOWN_BOND_ORDERS			= 1 << 1,
		COMPUTE_MISSING_SECONDARY_STRUCTURE = 1 << 2,
		ALL									= 0xFFu
	};

	/**
	 * @brief State of the loaded topology.
	 */
	enum struct TOPOLOGY_STATE : uint8_t
	{
		VTX_ENUM_ENABLE_BITMASK,
		OK					= 0,
		CHAIN_DEGENERATED	= 1 << 0,
		RESIDUE_DEGENERATED = 1 << 1,
		MISSING_CHAIN_INFO	= 1 << 2,
		MISSING_RESIDUES	= 1 << 3
	};
} // namespace VTX::IO

#endif
