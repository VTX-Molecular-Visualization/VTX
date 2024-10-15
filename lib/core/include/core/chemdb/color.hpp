#ifndef __VTX_CORE_CHEMDB_COLOR__
#define __VTX_CORE_CHEMDB_COLOR__

#include "atom.hpp"
#include "core/struct/color_layout.hpp"
#include "residue.hpp"
#include "secondary_structure.hpp"
#include <array>
#include <string>
#include <util/color/rgba.hpp>

namespace VTX::Core::ChemDB::Color
{
	constexpr int LAYOUT_OFFSET_ATOMS	 = 0;
	constexpr int LAYOUT_OFFSET_RESIDUES = 130;
	constexpr int LAYOUT_OFFSET_CHAINS	 = 175;
	constexpr int LAYOUT_OFFSET_RIBBONS	 = 230;
	constexpr int LAYOUT_OFFSET_CUSTOM	 = 245;

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
		RESIDUE_DC,
		RESIDUE_DG,
		RESIDUE_DA,
		RESIDUE_DU,
		RESIDUE_DT,
		RESIDUE_DI,
		RESIDUE_WAT,
		RESIDUE_HOH,

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
		CUSTOM_10
	};

	inline uchar getColorIndex( const Atom::SYMBOL p_symbol ) { return int( p_symbol ) + LAYOUT_OFFSET_ATOMS; }

	inline uchar getColorIndex( const Residue::SYMBOL p_symbol ) { return int( p_symbol ) + LAYOUT_OFFSET_RESIDUES; }

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

	inline uchar getColorIndex( const SecondaryStructure::TYPE p_type )
	{
		return int( p_type ) + LAYOUT_OFFSET_RIBBONS;
	}

	const Struct::ColorLayout COLOR_LAYOUT_JMOL = Struct::ColorLayout( { { 250, 22, 145 },	// UNKNOWN
																		 { 255, 255, 255 }, // H
																		 { 217, 255, 255 }, // HE
																		 { 204, 128, 255 }, // LI
																		 { 194, 255, 0 },	// BE
																		 { 255, 181, 181 }, // B
																		 { 144, 144, 144 }, // C
																		 { 48, 80, 248 },	// N
																		 { 255, 13, 13 },	// O
																		 { 144, 224, 80 },	// F
																		 { 179, 227, 245 }, // NE
																		 { 171, 92, 242 },	// NA
																		 { 138, 255, 0 },	// MG
																		 { 191, 166, 166 }, // AL
																		 { 240, 200, 160 }, // SI
																		 { 255, 128, 0 },	// P
																		 { 255, 255, 48 },	// S
																		 { 31, 240, 31 },	// CL
																		 { 128, 209, 227 }, // AR
																		 { 143, 64, 212 },	// K
																		 { 61, 255, 0 },	// CA
																		 { 230, 230, 230 }, // SC
																		 { 191, 194, 199 }, // TI
																		 { 166, 166, 171 }, // V
																		 { 138, 153, 199 }, // CR
																		 { 156, 122, 199 }, // MN
																		 { 224, 102, 51 },	// FE
																		 { 240, 144, 160 }, // CO
																		 { 80, 208, 80 },	// NI
																		 { 200, 128, 51 },	// CU
																		 { 125, 128, 176 }, // ZN
																		 { 194, 143, 143 }, // GA
																		 { 102, 143, 143 }, // GE
																		 { 189, 128, 227 }, // AS
																		 { 255, 161, 0 },	// SE
																		 { 166, 41, 41 },	// BR
																		 { 92, 184, 209 },	// KR
																		 { 112, 46, 176 },	// RB
																		 { 0, 255, 0 },		// SR
																		 { 148, 255, 255 }, // Y
																		 { 148, 224, 224 }, // ZR
																		 { 115, 194, 201 }, // NB
																		 { 84, 181, 181 },	// MO
																		 { 59, 158, 158 },	// TC
																		 { 36, 143, 143 },	// RU
																		 { 10, 125, 140 },	// RH
																		 { 0, 105, 133 },	// PD
																		 { 192, 192, 192 }, // AG
																		 { 255, 217, 143 }, // CD
																		 { 166, 117, 115 }, // IN
																		 { 102, 128, 128 }, // SN
																		 { 158, 99, 181 },	// SB
																		 { 212, 122, 0 },	// TE
																		 { 148, 0, 148 },	// I
																		 { 66, 158, 176 },	// XE
																		 { 87, 23, 143 },	// CS
																		 { 0, 201, 0 },		// BA
																		 { 112, 212, 255 }, // LA
																		 { 255, 255, 199 }, // CE
																		 { 217, 255, 199 }, // PR
																		 { 199, 255, 199 }, // ND
																		 { 163, 255, 199 }, // PM
																		 { 143, 255, 199 }, // SM
																		 { 97, 255, 199 },	// EU
																		 { 69, 255, 199 },	// GD
																		 { 48, 255, 199 },	// TB
																		 { 31, 255, 199 },	// DY
																		 { 0, 255, 156 },	// HO
																		 { 0, 230, 117 },	// ER
																		 { 0, 212, 82 },	// TM
																		 { 0, 191, 56 },	// YB
																		 { 0, 171, 36 },	// LU
																		 { 77, 194, 255 },	// HF
																		 { 77, 166, 255 },	// TA
																		 { 33, 148, 214 },	// W
																		 { 38, 125, 171 },	// RE
																		 { 38, 102, 150 },	// OS
																		 { 23, 84, 135 },	// IR
																		 { 208, 208, 224 }, // PT
																		 { 255, 209, 35 },	// AU
																		 { 184, 184, 208 }, // HG
																		 { 166, 84, 77 },	// TL
																		 { 87, 89, 97 },	// PB
																		 { 158, 79, 181 },	// BI
																		 { 171, 92, 0 },	// PO
																		 { 117, 79, 69 },	// AT
																		 { 66, 130, 150 },	// RN
																		 { 66, 0, 102 },	// FR
																		 { 0, 125, 0 },		// RA
																		 { 112, 171, 250 }, // AC
																		 { 0, 186, 255 },	// TH
																		 { 0, 161, 255 },	// PA
																		 { 0, 143, 255 },	// U
																		 { 0, 128, 255 },	// NP
																		 { 0, 107, 255 },	// PU
																		 { 84, 92, 242 },	// AM
																		 { 120, 92, 227 },	// CM
																		 { 138, 79, 227 },	// BK
																		 { 161, 54, 212 },	// CF
																		 { 179, 31, 212 },	// ES
																		 { 179, 31, 186 },	// FM
																		 { 179, 13, 16 },	// MD
																		 { 189, 13, 135 },	// NO
																		 { 199, 0, 102 },	// LR
																		 { 204, 0, 89 },	// RF
																		 { 209, 0, 79 },	// DD
																		 { 217, 0, 69 },	// SG
																		 { 224, 0, 56 },	// BHJ
																		 { 230, 0, 46 },	// HS
																		 { 235, 0, 38 },	// MT
																		 { 255, 255, 255 }, // DS
																		 { 255, 255, 255 }, // RG
																		 { 255, 255, 255 }, // UUB
																		 { 255, 255, 255 }, // UUT
																		 { 255, 255, 255 }, // UUQ
																		 { 255, 255, 255 }, // UUP
																		 { 255, 255, 255 }, // UUH
																		 { 255, 255, 255 }, // UUS
																		 { 255, 255, 255 }, // UUO
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},

																		 { 190, 160, 110 }, // UNKNOWN
																		 { 200, 200, 200 }, // ALA
																		 { 20, 90, 255 },	// ARG
																		 { 0, 220, 220 },	// ASN
																		 { 230, 10, 10 },	// ASP
																		 { 230, 230, 0 },	// CYS
																		 { 0, 220, 220 },	// GLN
																		 { 230, 10, 10 },	// GLU
																		 { 235, 235, 235 }, // GLY
																		 { 130, 130, 210 }, // HIS
																		 { 15, 130, 15 },	// ILE
																		 { 15, 130, 15 },	// LEU
																		 { 20, 90, 255 },	// LYS
																		 { 230, 230, 0 },	// MET
																		 { 50, 50, 170 },	// PHE
																		 { 220, 150, 130 }, // PRO
																		 { 250, 150, 0 },	// SER
																		 { 250, 150, 0 },	// THR
																		 { 180, 90, 180 },	// TRP
																		 { 50, 50, 170 },	// TYR
																		 { 15, 130, 15 },	// VAL
																		 { 190, 160, 110 }, // SEC
																		 { 190, 160, 110 }, // PYL
																		 { 255, 105, 180 }, // ASX
																		 { 255, 105, 180 }, // GLX
																		 { 255, 140, 75 },	// C
																		 { 255, 112, 112 }, // G
																		 { 160, 160, 255 }, // A
																		 { 255, 128, 128 }, // U
																		 { 128, 255, 255 }, // I
																		 { 255, 140, 75 },	// DC
																		 { 255, 112, 112 }, // DG
																		 { 160, 160, 255 }, // DA
																		 { 255, 128, 128 }, // DU
																		 { 160, 255, 160 }, // DT
																		 { 128, 255, 255 }, // DI
																		 { 255, 105, 180 }, // WAT
																		 { 255, 105, 180 }, // HOH
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},

																		 { 255, 255, 255 }, // Unknown
																		 { 192, 208, 255 }, // A, a,
																		 { 176, 255, 176 }, // B, b,
																		 { 255, 192, 200 }, // C, c,
																		 { 255, 255, 128 }, // D, d,
																		 { 255, 192, 255 }, // E, e,
																		 { 176, 240, 240 }, // F, f,
																		 { 255, 208, 112 }, // G, g,
																		 { 240, 128, 128 }, // H, h,
																		 { 245, 222, 179 }, // I, i,
																		 { 0, 191, 255 },	// J, j,
																		 { 205, 92, 92 },	// K, k,
																		 { 102, 205, 170 }, // L, l,
																		 { 154, 205, 50 },	// M, m,
																		 { 238, 130, 238 }, // N, n,
																		 { 0, 206, 209 },	// O, o,
																		 { 0, 255, 127 },	// P, p, 0,
																		 { 60, 179, 113 },	// Q, q, 1,
																		 { 0, 0, 139 },		// R, r, 2,
																		 { 189, 183, 107 }, // S, s, 3,
																		 { 0, 100, 0 },		// T, t, 4,
																		 { 128, 0, 0 },		// U, u, 5,
																		 { 128, 128, 0 },	// V, v, 6,
																		 { 128, 0, 128 },	// W, w, 7,
																		 { 0, 128, 128 },	// X, x, 8,
																		 { 184, 134, 11 },	// Y, y, 9,
																		 { 178, 34, 34 },	// Z, z
																		 { 144, 160, 207 }, // A, a,
																		 { 128, 207, 152 }, // B, b,
																		 { 207, 144, 176 }, // C, c,
																		 { 207, 207, 112 }, // D, d,
																		 { 207, 144, 207 }, // E, e,
																		 { 128, 192, 192 }, // F, f,
																		 { 207, 160, 96 },	// G, g,
																		 { 192, 80, 112 },	// H, h,
																		 { 197, 174, 131 }, // I, i,
																		 { 0, 167, 207 },	// J, j,
																		 { 181, 76, 76 },	// K, k,
																		 { 86, 181, 146 },	// L, l,
																		 { 138, 181, 42 },	// M, m,
																		 { 190, 114, 190 }, // N, n,
																		 { 0, 182, 161 },	// O, o,
																		 { 0, 207, 111 },	// P, p, 0,
																		 { 52, 155, 97 },	// Q, q, 1,
																		 { 0, 0, 187 },		// R, r, 2,
																		 { 165, 159, 91 },	// S, s, 3,
																		 { 0, 148, 0 },		// T, t, 4,
																		 { 176, 0, 0 },		// U, u, 5,
																		 { 176, 176, 0 },	// V, v, 6,
																		 { 176, 0, 176 },	// W, w, 7,
																		 { 0, 176, 176 },	// X, x, 8,
																		 { 232, 182, 19 },	// Y, y, 9,
																		 { 194, 50, 50 },	// Z, z
																		 {},
																		 {},

																		 { 1.f, 0.f, 0.5f },   // HELIX_ALPHA_RIGHT
																		 { 1.f, 0.f, 0.5f },   // HELIX_ALPHA_LEFT
																		 { 0.62f, 0.f, 0.5f }, // HELIX_3_10_RIGHT
																		 { 0.62f, 0.f, 0.5f }, // HELIX_3_10_LEFT
																		 { 0.37f, 0.f, 0.5f }, // HELIX_PI
																		 { 1.f, 0.78f, 0.f },  // STRAND
																		 { 0.37f, 0.5f, 1.f }, // TURN
																		 COLOR_WHITE,		   // COIL
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},
																		 {},

																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE,
																		 COLOR_WHITE } );

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
															  "Chain",		"Molecule",	   "Custom",	  "Inherited" };
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
} // namespace VTX::Core::ChemDB::Color

#endif
