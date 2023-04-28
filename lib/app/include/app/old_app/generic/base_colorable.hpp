#ifndef __VTX_BASE_COLORABLE__
#define __VTX_BASE_COLORABLE__

#include "util/color/rgba.hpp"
#include <util/types.hpp>
#include <string>
#include <vector>

namespace VTX
{
	namespace Generic
	{
		enum class COLOR_MODE : int
		{
			ATOM_CHAIN,
			ATOM_PROTEIN,
			ATOM_CUSTOM,
			RESIDUE,
			CHAIN,
			PROTEIN,
			CUSTOM,
			INHERITED
		};
		static const std::vector<std::string> COLOR_MODE_STRING { "Atom-Chain", "Atom-Object", "Atom-Custom",
																  "Residue",	"Chain",	   "Molecule",
																  "Custom",		"Inherited" };
		enum class COLOR_BLENDING_MODE : int
		{
			HARD,
			SOFT
		};
		inline static const std::vector<std::string> COLOR_BLENDING_MODE_STRING { "Hard", "Soft" };
		enum class SECONDARY_STRUCTURE_COLOR_MODE : int
		{
			JMOL,
			PROTEIN,
			CHAIN,
			RESIDUE,
			CUSTOM
		};
		inline static const std::vector<std::string> SECONDARY_STRUCTURE_COLOR_MODE_STRING { "JMOL",
																							 "Molecule",
																							 "Chain",
																							 "Residue",
																							 "Custom" };

		inline static const Util::Color::Rgba COLORS_JMOL[] = { Util::Color::Rgba( 1.f, 0.f, 0.5f ),   // HELIX_ALPHA_RIGHT
														  Util::Color::Rgba( 1.f, 0.f, 0.5f ),   // HELIX_ALPHA_LEFT
														  Util::Color::Rgba( 0.62f, 0.f, 0.5f ), // HELIX_3_10_RIGHT
														  Util::Color::Rgba( 0.62f, 0.f, 0.5f ), // HELIX_3_10_LEFT
														  Util::Color::Rgba( 0.37f, 0.f, 0.5f ), // HELIX_PI
														  Util::Color::Rgba( 1.f, 0.78f, 0.f ),  // STRAND
														  Util::Color::Rgba( 0.37f, 0.5f, 1.f ), // TURN
														  Util::Color::Rgba::WHITE };			   // COIL
		class BaseColorable
		{
		  public:
			inline const Util::Color::Rgba & getColor() const { return _color; }
			inline void				   setColor( const Util::Color::Rgba & p_color ) { _color = p_color; }

		  protected:
			Util::Color::Rgba _color = Util::Color::Rgba::random();
		};
	} // namespace Generic
} // namespace VTX
#endif
