#ifndef __ATOM2_HPP__
#define __ATOM2_HPP__

#include <vector>

#include "defines.hpp"

namespace vidocklab
{
	struct AMolecule;
	struct Chain;
	struct Residue;

	struct Atom
	{
		Atom();
		~Atom() = default;

		enum AtomType
		{
			H = 0,
			C,
			N,
			O,
			NA,
			MG,
			P,
			S,
			CA,
			// keep at last
			INVALID_TYPE,
			NB_ELEMENT_TYPES
		};

		inline const std::string &getName() const;
		inline const std::string &getSymbol() const;
		inline const int getAtomicNumber() const;
		inline const float getVdwRadius() const;
		inline const Vec3f &getColor() const;

		AMolecule *m_refMolecule = nullptr;
		Chain *m_refChain = nullptr;
		Residue *m_refResidue = nullptr;

		AtomType m_type = AtomType::INVALID_TYPE;
		uint m_id = INVALID_ID;

		// static data/methods
		static std::string s_names[NB_ELEMENT_TYPES];
		static std::string s_symbols[NB_ELEMENT_TYPES];
		static int s_atomicNumbers[NB_ELEMENT_TYPES];
		static float s_vdwRadii[NB_ELEMENT_TYPES];
		static Vec3f s_colors[NB_ELEMENT_TYPES];
		static bool s_initStatics;

		static void initAtomicNumbers();
		static void initNames();
		static void initSymbols();
		static void initVdwRadii(); // Van der Walls radii in Angstrom
		static void initColors();

		static AtomType getTypeFromSymbol(const std::string &s);

		static std::vector<std::string> s_unknownTypes;
	};

	inline const std::string &Atom::getName() const
	{
		return s_names[m_type];
	}

	inline const std::string &Atom::getSymbol() const
	{
		return s_symbols[m_type];
	}

	inline const int Atom::getAtomicNumber() const
	{
		return s_atomicNumbers[m_type];
	}

	inline const float Atom::getVdwRadius() const
	{
		return s_vdwRadii[m_type];
	}

	inline const Vec3f &Atom::getColor() const
	{
		return s_colors[m_type];
	}
} // namespace vidocklab

#endif // __ATOM2_HPP__
