/*
 * File: residue.hpp
 * Author : Maxime MARIA
 */

#ifndef __RESIDUE_HPP__
#define __RESIDUE_HPP__

#include <vector>

#include "defines.hpp"

namespace vidocklab
{
	struct AMolecule;
	struct Chain;

	struct Residue
	{
		Residue();
		~Residue() = default;

		enum ResidueType
		{
			ALA = 0,
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
			HOH, /// TODO: keep water as a residue ?
			// keep at last
			INVALID_TYPE,
			NB_RESIDUE_TYPES
		};

		inline const std::string &getName() const;
		inline const std::string &getLongSymbol() const;
		inline const std::string &getShortSymbol() const;
		inline const Vec3f &getColor() const;

		// Reference on parents in the hierarchy
		AMolecule *m_refMolecule = nullptr;
		Chain *m_refChain = nullptr;

		uint m_idFirstAtom = 0;
		uint m_nbAtoms = 0;
		uint m_idFirstBond = 0;
		uint m_nbBonds = 0;

		ResidueType m_type = ResidueType::INVALID_TYPE;
		uint m_id = INVALID_ID;

		// static data/methods
		static Vec3f s_colors[NB_RESIDUE_TYPES];
		static std::string s_names[NB_RESIDUE_TYPES];
		/// TODO: use map to allow multiple symbol ?
		static std::string s_longSymbols[NB_RESIDUE_TYPES];
		static std::string s_shortSymbols[NB_RESIDUE_TYPES];
		static bool s_initStatics;

		static void initNames();
		static void initLongSymbols();
		static void initShortSymbols();
		static void initColors();

		static ResidueType getTypeFromSymbol(const std::string &s);

		static std::vector<std::string> s_unknownTypes;
	};

	inline const std::string &Residue::getName() const
	{
		return s_names[m_type];
	}

	inline const std::string &Residue::getLongSymbol() const
	{
		return s_longSymbols[m_type];
	}

	inline const std::string &Residue::getShortSymbol() const
	{
		return s_shortSymbols[m_type];
	}

	inline const Vec3f &Residue::getColor() const
	{
		return s_colors[m_type];
	}

} // namespace vidocklab

#endif // __RESIDUE_HPP__
