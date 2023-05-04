#ifndef __VTX_APP_INTERNAL_CHEMDB_SECONDARY_STRUCTURE__
#define __VTX_APP_INTERNAL_CHEMDB_SECONDARY_STRUCTURE__

#include <util/color/rgba.hpp>

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

	inline static const Util::Color::Rgba COLORS_JMOL[] = { Util::Color::Rgba( 1.f, 0.f, 0.5f ),   // HELIX_ALPHA_RIGHT
															Util::Color::Rgba( 1.f, 0.f, 0.5f ),   // HELIX_ALPHA_LEFT
															Util::Color::Rgba( 0.62f, 0.f, 0.5f ), // HELIX_3_10_RIGHT
															Util::Color::Rgba( 0.62f, 0.f, 0.5f ), // HELIX_3_10_LEFT
															Util::Color::Rgba( 0.37f, 0.f, 0.5f ), // HELIX_PI
															Util::Color::Rgba( 1.f, 0.78f, 0.f ),  // STRAND
															Util::Color::Rgba( 0.37f, 0.5f, 1.f ), // TURN
															Util::Color::Rgba::WHITE };			   // COIL
} // namespace VTX::App::Internal::ChemDB::SecondaryStructure

#endif
