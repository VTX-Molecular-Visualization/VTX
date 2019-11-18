#include "residue.hpp"

namespace Model
{
	const std::string Residue::SYMBOL_NAME[ RESIDUE_SYMBOL::COUNT ] = {
		"Unknown",		 // UNKWNON,
		"Alanine",		 // ALA,
		"Arginine",		 // ARG,
		"Asparagine",	 // ASN,
		"Aspartate",	 // ASP,
		"Cysteine",		 // CYS,
		"Glutamine",	 // GLN,
		"Glutamate",	 // GLU,
		"Glycine",		 // GLY,
		"Histidine",	 // HIS,
		"Isoleucine",	 // ILE,
		"Leucine",		 // LEU,
		"Lysine",		 // LYS,
		"Methionine",	 // MET,
		"Phenylalanine", // PHE,
		"Proline",		 // PRO,
		"Serine",		 // SER,
		"Threonine",	 // THR,
		"Tryptophan",	 // TRP,
		"Tyrosine",		 // TYR,
		"Valine",		 // VAL,
		"Water"			 // HOH,
	};

	const std::string Residue::SYMBOL_SHORT[ RESIDUE_SYMBOL::COUNT ] = {
		"UNKNOWN", // UNKWNON
		"A",	   // ALA,
		"R",	   // ARG,
		"N",	   // ASN,
		"D",	   // ASP,
		"C",	   // CYS,
		"Q",	   // GLN,
		"E",	   // GLU,
		"G",	   // GLY,
		"H",	   // HIS,
		"I",	   // ILE,
		"L",	   // LEU,
		"K",	   // LYS,
		"M",	   // MET,
		"F",	   // PHE,
		"P",	   // PRO,
		"S",	   // SER,
		"T",	   // THR,
		"W",	   // TRP,
		"Y",	   // TYR,
		"V",	   // VAL,
		"HOH",	   // HOH,
	};

	const Vec3f * Residue::SYMBOL_COLOR[ RESIDUE_SYMBOL::COUNT ] = {
		&COLOR_WHITE
		// TODO
	};
} // namespace Model
