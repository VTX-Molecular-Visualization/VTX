/*
 * File: mmtfReader.hpp
 * Author : Maxime MARIA
 */

#ifndef __MMTF_READER_HPP__
#define __MMTF_READER_HPP__

#include <vector>

#include "MMTF/include/mmtf.hpp"

#include "defines.hpp"
#include "io/filePath.hpp"
#include "molecules/moleculeStatic.hpp"

namespace vidocklab
{
	class MMTFReader
	{
	public:
		// Read the MMTF file at <filePath> and load the first model
		// TODO reverse args (not const at the end)
		static void readFirstModel(MoleculeStatic &mol, const FilePath &fp);
	};
} // namespace vidocklab

#endif // __MMTF_READER_HPP__
