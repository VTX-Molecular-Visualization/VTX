/*
 * File: atom2.cpp
 * Author: Maxime MARIA
 */

#include "atom.hpp"

#include <algorithm>
#include <iostream>

namespace vidocklab
{
	// Static data
	int Atom::s_atomicNumbers[NB_ELEMENT_TYPES];
	std::string Atom::s_names[NB_ELEMENT_TYPES];
	std::string Atom::s_symbols[NB_ELEMENT_TYPES];
	float Atom::s_vdwRadii[NB_ELEMENT_TYPES];
	Vec3f Atom::s_colors[NB_ELEMENT_TYPES];
	bool Atom::s_initStatics = true;
	std::vector<std::string> Atom::s_unknownTypes;

	Atom::Atom()
	{
		if (s_initStatics)
		{
			initAtomicNumbers();
			initNames();
			initSymbols();
			initVdwRadii();
			initColors();
		}
	}

	void Atom::initAtomicNumbers()
	{
		s_atomicNumbers[AtomType::H] = 1;
		s_atomicNumbers[AtomType::C] = 6;
		s_atomicNumbers[AtomType::N] = 7;
		s_atomicNumbers[AtomType::O] = 8;
		s_atomicNumbers[AtomType::NA] = 11;
		s_atomicNumbers[AtomType::MG] = 12;
		s_atomicNumbers[AtomType::P] = 15;
		s_atomicNumbers[AtomType::S] = 16;
		s_atomicNumbers[AtomType::CA] = 20;
		s_atomicNumbers[AtomType::INVALID_TYPE] = -1;
	}

	void Atom::initNames()
	{
		s_names[AtomType::H] = "Hydrogen";
		s_names[AtomType::C] = "Carbon";
		s_names[AtomType::N] = "Nitrogen";
		s_names[AtomType::O] = "Oxygen";
		s_names[AtomType::NA] = "Sodium";
		s_names[AtomType::MG] = "Magnesium";
		s_names[AtomType::P] = "Phosphorus";
		s_names[AtomType::S] = "Sulfur";
		s_names[AtomType::CA] = "Calcium";
		s_names[AtomType::INVALID_TYPE] = UNDEFINED_STRING;
	}

	void Atom::initSymbols()
	{
		s_symbols[AtomType::H] = "H";
		s_symbols[AtomType::C] = "C";
		s_symbols[AtomType::N] = "N";
		s_symbols[AtomType::O] = "O";
		s_symbols[AtomType::NA] = "NA";
		s_symbols[AtomType::MG] = "MG";
		s_symbols[AtomType::P] = "P";
		s_symbols[AtomType::S] = "S";
		s_symbols[AtomType::CA] = "CA";
		s_symbols[AtomType::INVALID_TYPE] = UNDEFINED_STRING;
	}

	// data from "A cartography of the van der Waals territories"
	// S. Alvarez
	// https://pubs.rsc.org/en/content/articlehtml/2013/dt/c3dt50599e
	void Atom::initVdwRadii()
	{
		s_vdwRadii[AtomType::H] = 1.2f;
		s_vdwRadii[AtomType::C] = 1.77f;
		s_vdwRadii[AtomType::N] = 1.66f;
		s_vdwRadii[AtomType::O] = 1.5f;
		s_vdwRadii[AtomType::NA] = 2.5f;
		s_vdwRadii[AtomType::MG] = 2.51f;
		s_vdwRadii[AtomType::P] = 1.9f;
		s_vdwRadii[AtomType::S] = 1.89f;
		s_vdwRadii[AtomType::CA] = 2.62f;
		s_vdwRadii[AtomType::INVALID_TYPE] = 1.f;
	}

	// CPK convention
	void Atom::initColors()
	{
		s_colors[AtomType::H] = Vec3f(1.f, 1.f, 1.f);	// white
		s_colors[AtomType::C] = Vec3f(0.5f, 0.5f, 0.5f); // grey (black...)
		s_colors[AtomType::N] = Vec3f(0.13f, 0.2f, 1.f); // blue
		s_colors[AtomType::O] = Vec3f(1.f, 0.13f, 0.f);  // red
		s_colors[AtomType::NA] = Vec3f(0.47f, 0.f, 1.f); // violet
		s_colors[AtomType::MG] = Vec3f(0.f, 0.47f, 0.f); // dark green
		s_colors[AtomType::P] = Vec3f(1.f, 0.5f, 0.f);   // orange
		s_colors[AtomType::S] = Vec3f(1.f, 0.9f, 0.13f); // yellow
		s_colors[AtomType::CA]
			= Vec3f(1.f, 0.f, 1.f); // magenta cuase don't know
		s_colors[AtomType::INVALID_TYPE] = VEC3F_ZERO; // black
	}

	Atom::AtomType Atom::getTypeFromSymbol(const std::string &s)
	{
		for (int i = 0; i < NB_ELEMENT_TYPES; ++i)
		{
			if (s_symbols[i] == s)
			{
				return AtomType(i);
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
