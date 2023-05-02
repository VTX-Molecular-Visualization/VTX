#ifndef __VTX_APP_INTERNAL_CHEMDB_SECONDARY_STRUCTURE__
#define __VTX_APP_INTERNAL_CHEMDB_SECONDARY_STRUCTURE__

namespace VTX::App::Internal::ChemDB::SecondaryStructure
{
	enum class TYPE : int
	{
		HELIX_ALPHA_RIGHT = 0,
		HELIX_ALPHA_LEFT  = 1,
		HELIX_3_10_RIGHT  = 2,
		HELIX_3_10_LEFT	  = 3,
		HELIX_PI		  = 4,
		STRAND			  = 5,
		TURN			  = 6,
		COIL			  = 7,
		COUNT
	};
} // namespace VTX::App::Internal::ChemDB::SecondaryStructure

#endif
