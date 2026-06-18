#ifndef __VTX_IO_CONSTANTS__
#define __VTX_IO_CONSTANTS__

#include <string_view>
#include <util/enum.hpp>
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
		OK = 0,
		// Chain split into multiple blocks.
		CHAIN_DEGENERATED = 1 << 0,
		// Chain residues are unordered, or contain atom gaps.
		CHAIN_ORDER_DEGENERATED = 1 << 1,
		// Residue atoms not ordered.
		RESIDUE_DEGENERATED = 1 << 2,
		// No residues in source topology.
		MISSING_RESIDUES = 1 << 3,
		// Some atoms are not assigned to any residue.
		ATOM_RESIDUE_MEMBERSHIP = 1 << 4
	};
} // namespace VTX::IO

#endif
