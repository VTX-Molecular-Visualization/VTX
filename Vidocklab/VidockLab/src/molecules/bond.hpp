/*
 * File: bond.hpp
 * Author: Maxime MARIA
 */

#ifndef __BOND_HPP__
#define __BOND_HPP__

#include "defines.hpp"

namespace vidocklab
{
	class Molecule;
	class Atom;

	class Bond // comme James
	{
	public:
		Bond() = delete;
		Bond(const Molecule *refMol, const uint atomIdBegin,
			 const uint atomIdEnd);
		~Bond() = default;

		void setAtoms(const Atom &atomBegin, const Atom &atomEnd);

		inline uint getId() const;

	private:
		const Molecule *m_refMol;
		const uint m_id = INVALID_ID;

		uint m_atomIdBegin = -1;
		uint m_atomIdEnd = -1;
	};

	inline uint Bond::getId() const
	{
		return m_id;
	}
} // namespace vidocklab

#endif // __BOND_HPP__
