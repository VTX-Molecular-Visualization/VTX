#ifndef __VTX_APP_INTERNAL_CHEMDB_RESIDUE__
#define __VTX_APP_INTERNAL_CHEMDB_RESIDUE__

#include <string>
#include <util/color/rgba.hpp>

namespace VTX::Core::ChemDB::Residue
{
	enum class TYPE : int
	{
		STANDARD,
		NON_STANDARD
	};

	enum class SYMBOL_DISPLAY_MODE : int
	{
		SHORT,
		LONG,

		COUNT
	};

	enum class SYMBOL : int
	{
		UNKNOWN,
		ALA,
		ARG,
		ASN,
		ASP,
		CYS,
		GLN,
		GLU,
		GLY,
		HIS,
		ILE,
		LEU,
		LYS,
		MET,
		PHE,
		PRO,
		SER,
		THR,
		TRP,
		TYR,
		VAL,
		SEC,
		PYL,
		ASX,
		GLX,
		C,
		G,
		A,
		U,
		I,
		DC,
		DG,
		DA,
		DU,
		DT,
		DI,
		WAT,
		HOH,
		COUNT
	};

	const std::string SYMBOL_STR[ int( SYMBOL::COUNT ) ] = {
		"---", // UNKNOWN
		"ALA", // ALA
		"ARG", // ARG
		"ASN", // ASN
		"ASP", // ASP
		"CYS", // CYS
		"GLN", // GLN
		"GLU", // GLU
		"GLY", // GLY
		"HIS", // HIS
		"ILE", // ILE
		"LEU", // LEU
		"LYS", // LYS
		"MET", // MET
		"PHE", // PHE
		"PRO", // PRO
		"SER", // SER
		"THR", // THR
		"TRP", // TRP
		"TYR", // TYR
		"VAL", // VAL
		"SEC", // SEC
		"PYL", // PYL
		"ASX", // ASX
		"GLX", // GLX
		"C",   // C
		"G",   // G
		"A",   // A
		"U",   // U
		"I",   // I
		"DC",  // DC
		"DG",  // DG
		"DA",  // DA
		"DU",  // DU
		"DT",  // DT
		"DI",  // DI
		"WAT", // WAT
		"HOH", // HOH
	};

	const std::string SYMBOL_SHORT_STR[ int( SYMBOL::COUNT ) ] = {
		"-",  // UNKNOWN
		"A",  // ALA
		"R",  // ARG
		"N",  // ASN
		"D",  // ASP
		"C",  // CYS
		"Q",  // GLN
		"E",  // GLU
		"G",  // GLY
		"H",  // HIS
		"I",  // ILE
		"L",  // LEU
		"K",  // LYS
		"M",  // MET
		"F",  // PHE
		"P",  // PRO
		"S",  // SER
		"T",  // THR
		"W",  // TRP
		"Y",  // TYR
		"V",  // VAL
		"U",  // SEC
		"O",  // PYL
		"?",  // ASX
		"?",  // GLX
		"C",  // C
		"G",  // G
		"A",  // A
		"U",  // U
		"I",  // I
		"DC", // DC
		"DG", // DG
		"DA", // DA
		"DU", // DU
		"DT", // DT
		"DI", // DI
		"O",  // WAT
		"O",  // HOH
	};

	const std::string SYMBOL_NAME[ int( SYMBOL::COUNT ) ] = {
		"Unknown",		  // UNKNOWN
		"Alanine",		  // ALA
		"Arginine",		  // ARG
		"Asparagine",	  // ASN
		"Aspartate",	  // ASP
		"Cysteine",		  // CYS
		"Glutamine",	  // GLN
		"Glutamate",	  // GLU
		"Glycine",		  // GLY
		"Histidine",	  // HIS
		"Isoleucine",	  // ILE
		"Leucine",		  // LEU
		"Lysine",		  // LYS
		"Methionine",	  // MET
		"Phenylalanine",  // PHE
		"Proline",		  // PRO
		"Serine",		  // SER
		"Threonine",	  // THR
		"Tryptophan",	  // TRP
		"Tyrosine",		  // TYR
		"Valine",		  // VAL
		"Selenocysteine", // SEC
		"Pyrrolysine",	  // PYL
		"?",			  // ASX
		"?",			  // GLX
		"Cytosine",		  // C
		"Guanine",		  // G
		"Adenine",		  // A
		"Uracil",		  // U
		"Inosine",		  // I
		"Cytosine",		  // DC
		"Guanine",		  // DG
		"Adenine",		  // DA
		"Uracil",		  // DU
		"Thymine",		  // DT
		"Inosine",		  // DI
		"Water",		  // WAT
		"Water",		  // HOH
	};

	// http://jmol.sourceforge.net/jscolors/#Jmolcolors : Protein "amino" colors
	const Util::Color::Rgba SYMBOL_COLOR[ int( SYMBOL::COUNT ) ] = {
		{ 190, 160, 110 }, // UNKNOWN
		{ 200, 200, 200 }, // ALA
		{ 20, 90, 255 },   // ARG
		{ 0, 220, 220 },   // ASN
		{ 230, 10, 10 },   // ASP
		{ 230, 230, 0 },   // CYS
		{ 0, 220, 220 },   // GLN
		{ 230, 10, 10 },   // GLU
		{ 235, 235, 235 }, // GLY
		{ 130, 130, 210 }, // HIS
		{ 15, 130, 15 },   // ILE
		{ 15, 130, 15 },   // LEU
		{ 20, 90, 255 },   // LYS
		{ 230, 230, 0 },   // MET
		{ 50, 50, 170 },   // PHE
		{ 220, 150, 130 }, // PRO
		{ 250, 150, 0 },   // SER
		{ 250, 150, 0 },   // THR
		{ 180, 90, 180 },  // TRP
		{ 50, 50, 170 },   // TYR
		{ 15, 130, 15 },   // VAL
		{ 190, 160, 110 }, // SEC
		{ 190, 160, 110 }, // PYL
		{ 255, 105, 180 }, // ASX
		{ 255, 105, 180 }, // GLX
		{ 255, 140, 75 },  // C
		{ 255, 112, 112 }, // G
		{ 160, 160, 255 }, // A
		{ 255, 128, 128 }, // U
		{ 128, 255, 255 }, // I
		{ 255, 140, 75 },  // DC
		{ 255, 112, 112 }, // DG
		{ 160, 160, 255 }, // DA
		{ 255, 128, 128 }, // DU
		{ 160, 255, 160 }, // DT
		{ 128, 255, 255 }, // DI
		{ 255, 105, 180 }, // WAT
		{ 255, 105, 180 }, // HOH
	};

} // namespace VTX::Core::ChemDB::Residue

#endif
