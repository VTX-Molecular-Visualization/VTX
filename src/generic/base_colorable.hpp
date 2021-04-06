#ifndef __VTX_BASE_COLORABLE__
#define __VTX_BASE_COLORABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "color/rgb.hpp"
#include "define.hpp"
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
																  "Residue",	"Chain",	   "Object",
																  "Custom",		"Inherited" };

		enum class SECONDARY_STRUCTURE_COLOR_MODE : int
		{
			JMOL,
			PROTEIN,
			CHAIN,
			RESIDUE,
			CUSTOM
		};
		inline static const std::vector<std::string> SECONDARY_STRUCTURE_COLOR_MODE_STRING { "JMOL",
																							 "Object",
																							 "Chain",
																							 "Residue",
																							 "Custom" };

		inline static const Color::Rgb COLORS_JMOL[] = { Color::Rgb( 1.f, 0.f, 0.5f ),	 // HELIX_ALPHA_RIGHT
														 Color::Rgb( 1.f, 0.f, 0.5f ),	 // HELIX_ALPHA_LEFT
														 Color::Rgb( 0.62f, 0.f, 0.5f ), // HELIX_3_10_RIGHT
														 Color::Rgb( 0.62f, 0.f, 0.5f ), // HELIX_3_10_LEFT
														 Color::Rgb( 0.37f, 0.f, 0.5f ), // HELIX_PI
														 Color::Rgb( 1.f, 0.78f, 0.f ),	 // STRAND
														 Color::Rgb( 0.37f, 0.5f, 1.f ), // TURN
														 Color::Rgb::WHITE };			 // COIL
		class BaseColorable
		{
		  public:
			inline const Color::Rgb & getColor() const { return _color; }
			inline void				  setColor( const Color::Rgb & p_color ) { _color = p_color; }

		  protected:
			Color::Rgb _color = Color::Rgb::random();
		};
	} // namespace Generic
} // namespace VTX
#endif
