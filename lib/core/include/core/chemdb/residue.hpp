#ifndef VTX_CORE_CHEMDB_RESIDUE_HPP
#define VTX_CORE_CHEMDB_RESIDUE_HPP

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace VTX::Core::ChemDB::Residue
{
	enum class TYPE : uint8_t
	{
		STANDARD,
		NON_STANDARD
	};

	enum class SYMBOL_DISPLAY_MODE : uint8_t
	{
		SHORT,
		LONG,
	};

	enum class SYMBOL : uint8_t
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

	constexpr auto SYMBOL_STR = std::to_array<std::string_view>( {
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
	} );
	static_assert( SYMBOL_STR.size() == static_cast<size_t>( SYMBOL::COUNT ) );

	constexpr auto SYMBOL_SHORT_STR = std::to_array<std::string_view>( {
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
	} );
	static_assert( SYMBOL_SHORT_STR.size() == static_cast<size_t>( SYMBOL::COUNT ) );

	constexpr auto SYMBOL_NAME = std::to_array<std::string_view>( {
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
	} );
	static_assert( SYMBOL_NAME.size() == static_cast<size_t>( SYMBOL::COUNT ) );

	constexpr auto SYMBOL_IS_COMMON = std::to_array<bool>( {
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

	} );
	static_assert( SYMBOL_IS_COMMON.size() == static_cast<size_t>( SYMBOL::COUNT ) );

	SYMBOL getSymbolFromShortName( const std::string & p_residueName );
	SYMBOL getSymbolFromName( const std::string & p_residueName );
	SYMBOL getSymbolFromLongName( const std::string & p_residueName );
	SYMBOL getSymbolFromAnyName( const std::string & p_residueName );

	bool checkIfStandardFromName( const std::string & p_residueSymbol );

} // namespace VTX::Core::ChemDB::Residue

#endif
