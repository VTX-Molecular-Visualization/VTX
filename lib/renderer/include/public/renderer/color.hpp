#ifndef __VTX_RENDERER_COLOR__
#define __VTX_RENDERER_COLOR__

#include <array>
#include <core/chemdb/atom.hpp>
#include <core/chemdb/residue.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <util/color/rgba.hpp>

namespace VTX::Renderer::Color
{
	constexpr size_t COLOR_LAYOUT_SIZE = 256;
	using LayoutArray				   = std::array<Util::Color::Rgba, COLOR_LAYOUT_SIZE>;

	struct Layout
	{
		LayoutArray colors;
	};

	constexpr int LAYOUT_OFFSET_ATOMS	 = 0;
	constexpr int LAYOUT_COUNT_ATOMS	 = int( Core::ChemDB::Atom::SYMBOL::COUNT );
	constexpr int LAYOUT_OFFSET_RESIDUES = 121;
	constexpr int LAYOUT_COUNT_RESIDUES	 = int( Core::ChemDB::Residue::SYMBOL::COUNT );
	constexpr int LAYOUT_OFFSET_CHAINS	 = 163;
	constexpr int LAYOUT_COUNT_CHAINS	 = 53;
	constexpr int LAYOUT_OFFSET_RIBBONS	 = 218;
	constexpr int LAYOUT_COUNT_RIBBONS	 = 8;
	constexpr int LAYOUT_OFFSET_CUSTOM	 = 228;
	constexpr int LAYOUT_COUNT_CUSTOM	 = 14;

	enum class E_LAYOUT_COLOR_INDEX : uchar
	{
		ATOM_UNKNOWN = LAYOUT_OFFSET_ATOMS,
		ATOM_H,
		ATOM_HE,
		ATOM_LI,
		ATOM_BE,
		ATOM_B,
		ATOM_C,
		ATOM_N,
		ATOM_O,
		ATOM_F,
		ATOM_NE,
		ATOM_NA,
		ATOM_MG,
		ATOM_AL,
		ATOM_SI,
		ATOM_P,
		ATOM_S,
		ATOM_CL,
		ATOM_AR,
		ATOM_K,
		ATOM_CA,
		ATOM_SC,
		ATOM_TI,
		ATOM_V,
		ATOM_CR,
		ATOM_MN,
		ATOM_FE,
		ATOM_CO,
		ATOM_NI,
		ATOM_CU,
		ATOM_ZN,
		ATOM_GA,
		ATOM_GE,
		ATOM_AS,
		ATOM_SE,
		ATOM_BR,
		ATOM_KR,
		ATOM_RB,
		ATOM_SR,
		ATOM_Y,
		ATOM_ZR,
		ATOM_NB,
		ATOM_MO,
		ATOM_TC,
		ATOM_RU,
		ATOM_RH,
		ATOM_PD,
		ATOM_AG,
		ATOM_CD,
		ATOM_IN,
		ATOM_SN,
		ATOM_SB,
		ATOM_TE,
		ATOM_I,
		ATOM_XE,
		ATOM_CS,
		ATOM_BA,
		ATOM_LA,
		ATOM_CE,
		ATOM_PR,
		ATOM_ND,
		ATOM_PM,
		ATOM_SM,
		ATOM_EU,
		ATOM_GD,
		ATOM_TB,
		ATOM_DY,
		ATOM_HO,
		ATOM_ER,
		ATOM_TM,
		ATOM_YB,
		ATOM_LU,
		ATOM_HF,
		ATOM_TA,
		ATOM_W,
		ATOM_RE,
		ATOM_OS,
		ATOM_IR,
		ATOM_PT,
		ATOM_AU,
		ATOM_HG,
		ATOM_TL,
		ATOM_PB,
		ATOM_BI,
		ATOM_PO,
		ATOM_AT,
		ATOM_RN,
		ATOM_FR,
		ATOM_RA,
		ATOM_AC,
		ATOM_TH,
		ATOM_PA,
		ATOM_U,
		ATOM_NP,
		ATOM_PU,
		ATOM_AM,
		ATOM_CM,
		ATOM_BK,
		ATOM_CF,
		ATOM_ES,
		ATOM_FM,
		ATOM_MD,
		ATOM_NO,
		ATOM_LR,
		ATOM_RF,
		ATOM_DD,
		ATOM_SG,
		ATOM_BHJ,
		ATOM_HS,
		ATOM_MT,
		ATOM_DS,
		ATOM_RG,
		ATOM_UUB,
		ATOM_UUT,
		ATOM_UUQ,
		ATOM_UUP,
		ATOM_UUH,
		ATOM_UUS,
		ATOM_UUO,

		RESIDUE_UNKNOWN = LAYOUT_OFFSET_RESIDUES,
		RESIDUE_ALA,
		RESIDUE_ARG,
		RESIDUE_ASN,
		RESIDUE_ASP,
		RESIDUE_CYS,
		RESIDUE_GLN,
		RESIDUE_GLU,
		RESIDUE_GLY,
		RESIDUE_HIS,
		RESIDUE_ILE,
		RESIDUE_LEU,
		RESIDUE_LYS,
		RESIDUE_MET,
		RESIDUE_PHE,
		RESIDUE_PRO,
		RESIDUE_SER,
		RESIDUE_THR,
		RESIDUE_TRP,
		RESIDUE_TYR,
		RESIDUE_VAL,
		RESIDUE_SEC,
		RESIDUE_PYL,
		RESIDUE_ASX,
		RESIDUE_GLX,
		RESIDUE_C,
		RESIDUE_G,
		RESIDUE_A,
		RESIDUE_U,
		RESIDUE_I,
		RESIDUE_T,
		RESIDUE_DC,
		RESIDUE_DG,
		RESIDUE_DA,
		RESIDUE_DU,
		RESIDUE_DT,
		RESIDUE_DI,
		RESIDUE_WAT,
		RESIDUE_HOH,
		RESIDUE_SOL,

		CHAIN_UNKNOWM = LAYOUT_OFFSET_CHAINS,
		CHAIN_0,
		CHAIN_1,
		CHAIN_2,
		CHAIN_3,
		CHAIN_4,
		CHAIN_5,
		CHAIN_6,
		CHAIN_7,
		CHAIN_8,
		CHAIN_9,
		CHAIN_10,
		CHAIN_11,
		CHAIN_12,
		CHAIN_13,
		CHAIN_14,
		CHAIN_15,
		CHAIN_16,
		CHAIN_17,
		CHAIN_18,
		CHAIN_19,
		CHAIN_20,
		CHAIN_21,
		CHAIN_22,
		CHAIN_23,
		CHAIN_24,
		CHAIN_25,
		CHAIN_HETATM_0,
		CHAIN_HETATM_1,
		CHAIN_HETATM_2,
		CHAIN_HETATM_3,
		CHAIN_HETATM_4,
		CHAIN_HETATM_5,
		CHAIN_HETATM_6,
		CHAIN_HETATM_7,
		CHAIN_HETATM_8,
		CHAIN_HETATM_9,
		CHAIN_HETATM_10,
		CHAIN_HETATM_11,
		CHAIN_HETATM_12,
		CHAIN_HETATM_13,
		CHAIN_HETATM_14,
		CHAIN_HETATM_15,
		CHAIN_HETATM_16,
		CHAIN_HETATM_17,
		CHAIN_HETATM_18,
		CHAIN_HETATM_19,
		CHAIN_HETATM_20,
		CHAIN_HETATM_21,
		CHAIN_HETATM_22,
		CHAIN_HETATM_23,
		CHAIN_HETATM_24,
		CHAIN_HETATM_25,

		RIBBON_HELIX_ALPHA_RIGHT = LAYOUT_OFFSET_RIBBONS,
		RIBBON_HELIX_ALPHA_LEFT,
		RIBBON_HELIX_3_10_RIGHT,
		RIBBON_HELIX_3_10_LEFT,
		RIBBON_HELIX_PI,
		RIBBON_STRAND,
		RIBBON_TURN,
		RIBBON_COIL,

		CUSTOM_0 = LAYOUT_OFFSET_CUSTOM,
		CUSTOM_1,
		CUSTOM_2,
		CUSTOM_3,
		CUSTOM_4,
		CUSTOM_5,
		CUSTOM_6,
		CUSTOM_7,
		CUSTOM_8,
		CUSTOM_9,
		CUSTOM_10,
		CUSTOM_11,
		CUSTOM_12,
		CUSTOM_13,
		CUSTOM_14,
		CUSTOM_15,
		CUSTOM_16,
		CUSTOM_17,
		CUSTOM_18,
		CUSTOM_19,
		CUSTOM_20,
		CUSTOM_21,
		CUSTOM_22,
		CUSTOM_23,
		CUSTOM_24,
		CUSTOM_25,
		CUSTOM_26,
		CUSTOM_27
	};

	inline uchar getColorIndex( const Core::ChemDB::Atom::SYMBOL p_symbol )
	{
		return int( p_symbol ) + LAYOUT_OFFSET_ATOMS;
	}

	inline uchar getColorIndex( const Core::ChemDB::Residue::SYMBOL p_symbol )
	{
		return int( p_symbol ) + LAYOUT_OFFSET_RESIDUES;
	}

	inline uchar getColorIndex(

		const std::string & p_chainId,
		const bool			p_isHetAtm = false
	)
	{
		if ( p_chainId.empty() )
		{
			return char( E_LAYOUT_COLOR_INDEX::CHAIN_UNKNOWM );
		}

		// chain id should be defined by one char
		const char c = static_cast<char>( std::toupper( static_cast<unsigned char>( p_chainId[ 0 ] ) ) );

		const int id = int( c ) - 65; // 65 is A
		if ( id < 0 || id > 26 )
		{
			return char( E_LAYOUT_COLOR_INDEX::CHAIN_UNKNOWM );
		}

		return p_isHetAtm ? id + LAYOUT_OFFSET_CHAINS + 1 + 26 : id + LAYOUT_OFFSET_CHAINS + 1;
	}

	inline uchar getColorIndex( const Core::ChemDB::SecondaryStructure::TYPE p_type )
	{
		return int( p_type ) + LAYOUT_OFFSET_RIBBONS;
	}

	/*
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
static const std::vector<std::string> COLOR_MODE_STRING { "Atom-Chain", "Atom-Object", "Atom-Custom", "Residue",
														  "Chain",		"System",	   "Custom",	  "Inherited" };
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
																					 "System",
																					 "Chain",
																					 "Residue",
																					 "Custom" };
																					 */
} // namespace VTX::Renderer::Color

#endif
