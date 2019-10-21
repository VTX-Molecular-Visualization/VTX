/*
 * File: residue.cpp
 * Author: Maxime MARIA
 */

#include "residue.hpp"

#include <algorithm>
#include <iostream>

namespace vidocklab
{
	std::string Residue::s_names[NB_RESIDUE_TYPES];
	std::string Residue::s_longSymbols[NB_RESIDUE_TYPES];
	std::string Residue::s_shortSymbols[NB_RESIDUE_TYPES];
	Vec3f Residue::s_colors[NB_RESIDUE_TYPES];
	bool Residue::s_initStatics = true;
	std::vector<std::string> Residue::s_unknownTypes;

	Residue::Residue()
	{
		if (s_initStatics)
		{
			initNames();
			initLongSymbols();
			initShortSymbols();
			initColors();
		}
	}

	void Residue::initNames()
	{
		s_names[ResidueType::ALA] = "Alanine";
		s_names[ResidueType::ARG] = "Arginine";
		s_names[ResidueType::ASN] = "Asparagine";
		s_names[ResidueType::ASP] = "Aspartate";
		s_names[ResidueType::CYS] = "Cysteine";
		s_names[ResidueType::GLN] = "Glutamine";
		s_names[ResidueType::GLU] = "Glutamate";
		s_names[ResidueType::GLY] = "Glycine";
		s_names[ResidueType::HIS] = "Histidine";
		s_names[ResidueType::ILE] = "Isoleucine";
		s_names[ResidueType::LEU] = "Leucine";
		s_names[ResidueType::LYS] = "Lysine";
		s_names[ResidueType::MET] = "Methionine";
		s_names[ResidueType::PHE] = "Phenylalanine";
		s_names[ResidueType::PRO] = "Proline";
		s_names[ResidueType::SER] = "Serine";
		s_names[ResidueType::THR] = "Threonine";
		s_names[ResidueType::TRP] = "Tryptophan";
		s_names[ResidueType::TYR] = "Tyrosine";
		s_names[ResidueType::VAL] = "Valine";
		s_names[ResidueType::HOH] = "Water";
		s_names[ResidueType::INVALID_TYPE] = UNDEFINED_STRING;
	}

	void Residue::initLongSymbols()
	{
		s_longSymbols[ResidueType::ALA] = "ALA";
		s_longSymbols[ResidueType::ARG] = "ARG";
		s_longSymbols[ResidueType::ASN] = "ASN";
		s_longSymbols[ResidueType::ASP] = "ASP";
		s_longSymbols[ResidueType::CYS] = "CYS";
		s_longSymbols[ResidueType::GLN] = "GLN";
		s_longSymbols[ResidueType::GLU] = "GLU";
		s_longSymbols[ResidueType::GLY] = "GLY";
		s_longSymbols[ResidueType::HIS] = "HIS";
		s_longSymbols[ResidueType::ILE] = "ILE";
		s_longSymbols[ResidueType::LEU] = "LEU";
		s_longSymbols[ResidueType::LYS] = "LYS";
		s_longSymbols[ResidueType::MET] = "MET";
		s_longSymbols[ResidueType::PHE] = "PHE";
		s_longSymbols[ResidueType::PRO] = "PRO";
		s_longSymbols[ResidueType::SER] = "SER";
		s_longSymbols[ResidueType::THR] = "THR";
		s_longSymbols[ResidueType::TRP] = "TRP";
		s_longSymbols[ResidueType::TYR] = "TYR";
		s_longSymbols[ResidueType::VAL] = "VAL";
		s_longSymbols[ResidueType::HOH] = "HOH";
		s_longSymbols[ResidueType::INVALID_TYPE] = UNDEFINED_STRING;
	}

	void Residue::initShortSymbols()
	{
		s_shortSymbols[ResidueType::ALA] = "A";
		s_shortSymbols[ResidueType::ARG] = "R";
		s_shortSymbols[ResidueType::ASN] = "N";
		s_shortSymbols[ResidueType::ASP] = "D";
		s_shortSymbols[ResidueType::CYS] = "C";
		s_shortSymbols[ResidueType::GLN] = "Q";
		s_shortSymbols[ResidueType::GLU] = "E";
		s_shortSymbols[ResidueType::GLY] = "G";
		s_shortSymbols[ResidueType::HIS] = "H";
		s_shortSymbols[ResidueType::ILE] = "I";
		s_shortSymbols[ResidueType::LEU] = "L";
		s_shortSymbols[ResidueType::LYS] = "K";
		s_shortSymbols[ResidueType::MET] = "M";
		s_shortSymbols[ResidueType::PHE] = "F";
		s_shortSymbols[ResidueType::PRO] = "P";
		s_shortSymbols[ResidueType::SER] = "S";
		s_shortSymbols[ResidueType::THR] = "T";
		s_shortSymbols[ResidueType::TRP] = "W";
		s_shortSymbols[ResidueType::TYR] = "Y";
		s_shortSymbols[ResidueType::VAL] = "V";
		s_shortSymbols[ResidueType::HOH] = "HOH";
		s_shortSymbols[ResidueType::INVALID_TYPE] = UNDEFINED_STRING;
	}

	// according to rasmol coloring : http://www.openrasmol.org/
	void Residue::initColors()
	{
		s_colors[ResidueType::ALA] = Vec3f(0.78f, 0.78f, 0.78f); // dark grey
		s_colors[ResidueType::ARG] = s_colors[ResidueType::LYS]
			= Vec3f(0.08f, 0.35f, 0.f); // blue
		s_colors[ResidueType::ASN] = s_colors[ResidueType::GLN]
			= Vec3f(0.f, 0.86f, 0.86f); // cyan
		s_colors[ResidueType::ASP] = s_colors[ResidueType::GLU]
			= Vec3f(0.9f, 0.04f, 0.04f); // bright red
		s_colors[ResidueType::CYS] = s_colors[ResidueType::MET]
			= Vec3f(0.9f, 0.9f, 0.f);							 // yellow
		s_colors[ResidueType::GLY] = Vec3f(0.92f, 0.92f, 0.92f); // light grey
		s_colors[ResidueType::HIS] = Vec3f(0.51f, 0.51f, 0.82f); // pale blue
		s_colors[ResidueType::ILE] = s_colors[ResidueType::LEU]
			= s_colors[ResidueType::VAL] = Vec3f(0.06f, 0.51f, 0.06f); // green
		s_colors[ResidueType::PHE] = s_colors[ResidueType::TYR]
			= Vec3f(0.2f, 0.2f, 0.67f);							 // mid blue
		s_colors[ResidueType::PRO] = Vec3f(0.86f, 0.59f, 0.51f); // flesh
		s_colors[ResidueType::SER] = s_colors[ResidueType::THR]
			= Vec3f(0.98f, 0.59f, 0.f);							 // orange
		s_colors[ResidueType::TRP] = Vec3f(0.71f, 0.35f, 0.71f); // pink
		s_colors[ResidueType::HOH]
			= Vec3f(0.47f, 0.0f, 1.f); // purple (not from rasmol)
		s_colors[ResidueType::INVALID_TYPE] = VEC3F_ZERO; // black
	}

	Residue::ResidueType Residue::getTypeFromSymbol(const std::string &s)
	{
		for (int i = 0; i < NB_RESIDUE_TYPES; ++i)
		{
			if (s_longSymbols[i] == s || s_shortSymbols[i] == s)
			{
				return ResidueType(i);
			}
		}

		std::vector<std::string>::const_iterator it
			= std::find(s_unknownTypes.begin(), s_unknownTypes.end(), s);
		if (it == s_unknownTypes.end())
		{
			s_unknownTypes.emplace_back(s);
		}
		return INVALID_TYPE;
	}
} // namespace vidocklab
