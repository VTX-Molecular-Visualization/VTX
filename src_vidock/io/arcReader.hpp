/*
 * File: arcReader.hpp
 * Author : Maxime MARIA
 *
 * Static class to read .arc files
 * Molecular dynamics result:
 * - atom positions
 * - bonds
 * - atom types
 */

#ifndef __ARC_READER_HPP__
#define __ARC_READER_HPP__

#include "io/filePath.hpp"

#include "molecules/moleculeDynamic.hpp"

namespace vidocklab
{
	class ArcReader
	{
	public:
		// Read the .arc file at <filePath> and load the the molecular dynamics
		// frames
		static void read(const FilePath &fp, MoleculeDynamic &mol);

	private:
		// remove B/A when A/B exists
		static void removeDuplicatedBonds(std::vector<uint> &bonds);
		// /TODO remove
		static uint m_cptRemoved;
	};
} // namespace vidocklab

#endif // __MMTF_READER_HPP__
