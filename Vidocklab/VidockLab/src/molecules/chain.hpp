/*
 * File: chain.hpp
 * Author : Maxime MARIA
 */

#ifndef __CHAIN_HPP__
#define __CHAIN_HPP__

#include <vector>

#include "defines.hpp"

namespace vidocklab
{
	struct AMolecule;

	struct Chain
	{
		AMolecule *m_refMolecule = nullptr;

		std::string m_name = UNDEFINED_STRING;
		uint m_idFirstResidue = 0;
		uint m_nbResidues = 0;
		uint m_id = INVALID_ID;
	};
} // namespace vidocklab

#endif // __CHAIN_HPP__
