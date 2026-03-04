#ifndef __VTX_CORE_CHEMDB_BOND__
#define __VTX_CORE_CHEMDB_BOND__

#include <array>
#include <string>
#include <util/color/rgba.hpp>

namespace VTX::Core::ChemDB::Bond
{
	enum class ORDER : uint8_t
	{
		UNKNOWN	   = 0, ///< Bond order is unknown or unspecified
		SINGLE	   = 1, ///< Single bond
		DOUBLE	   = 2, ///< Double bond
		TRIPLE	   = 3, ///< Triple bond
		QUADRUPLE  = 4, ///< Quadruplet bond
		QUINTUPLET = 5, ///< Quintuplet bond
		AROMATIC   = 6, ///< Aromatic bond
		COUNT,

		// space for more bond types if needed
		// DOWN = 250, ///< Single bond direction from first atom to second is 'down'. Used for cis-trans
		// isomers UP	 = 251, ///< Single bond direction from first atom to second is 'up'. Used for cis-trans
		// isomers DATIVE_R = 252, ///< Dative bond where the electrons are localized to the first atom DATIVE_L
		// = 253, ///< Dative bond where the electrons are localized to the second atom
		// AMIDE	 = 254, ///< Amide bond (C(=O)-NH)
		// AROMATIC = 255, ///< Aromatic bond (for example the ring bonds in benzene)
	};

	constexpr std::array<std::string_view, size_t( ORDER::COUNT )> ORDER_STR
		= { "Unknown", "Simple", "Double", "Triple", "Quadruple", "Quintuple", "Aromatique" };
} // namespace VTX::Core::ChemDB::Bond

#endif
