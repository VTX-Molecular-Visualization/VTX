#ifndef __VTX_CORE_CHEMDB_RESIDUE__
#define __VTX_CORE_CHEMDB_RESIDUE__

#include <array>
#include <string>

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
		T,
		DC,
		DG,
		DA,
		DU,
		DT,
		DI,
		WAT,
		HOH,
		SOL,
		COUNT
	};

	constexpr std::array<std::string_view, size_t( SYMBOL::COUNT )> SYMBOL_STR = {
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
		"T",   // T
		"DC",  // DC
		"DG",  // DG
		"DA",  // DA
		"DU",  // DU
		"DT",  // DT
		"DI",  // DI
		"WAT", // WAT
		"HOH", // HOH
		"SOL", // SOL
	};

	constexpr std::array<std::string_view, size_t( SYMBOL::COUNT )> SYMBOL_SHORT_STR {
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
		"T",  // T
		"DC", // DC
		"DG", // DG
		"DA", // DA
		"DU", // DU
		"DT", // DT
		"DI", // DI
		"O",  // WAT
		"O",  // HOH
		"S",  // SOL
	};

	constexpr std::array<std::string_view, size_t( SYMBOL::COUNT )> SYMBOL_NAME = {
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
		"Thymine",		  // T
		"Cytosine",		  // DC
		"Guanine",		  // DG
		"Adenine",		  // DA
		"Uracil",		  // DU
		"Thymine",		  // DT
		"Inosine",		  // DI
		"Water",		  // WAT
		"Water",		  // HOH
		"Solvent",		  // SOL
	};

	constexpr std::array<bool, size_t( SYMBOL::COUNT )> SYMBOL_IS_COMMON = {
		true,  // UNKNOWN
		true,  // ALA
		true,  // ARG
		true,  // ASN
		true,  // ASP
		true,  // CYS
		true,  // GLN
		true,  // GLU
		true,  // GLY
		true,  // HIS
		true,  // ILE
		true,  // LEU
		true,  // LYS
		true,  // MET
		true,  // PHE
		true,  // PRO
		true,  // SER
		true,  // THR
		true,  // TRP
		true,  // TYR
		true,  // VAL
		false, // SEC
		false, // PYL
		false, // ASX
		false, // GLX
		true,  // C
		true,  // G
		true,  // A
		true,  // U
		true,  // I
		true,  // T
		false, // DC
		false, // DG
		false, // DA
		false, // DU
		false, // DT
		false, // DI
		true,  // WAT
		true,  // HOH
		true,  // SOL

	};

	const SYMBOL getSymbolFromShortName( const std::string & p_residueName );
	const SYMBOL getSymbolFromName( const std::string & p_residueName );
	const SYMBOL getSymbolFromLongName( const std::string & p_residueName );
	const SYMBOL getSymbolFromAnyName( const std::string & p_residueName );

	bool checkIfStandardFromName( const std::string & p_residueSymbol );

} // namespace VTX::Core::ChemDB::Residue

#endif
