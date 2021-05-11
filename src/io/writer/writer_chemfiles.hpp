#ifndef __VTX_WRITE_CHEMFILES__
#define __VTX_WRITE_CHEMFILES__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include <chemfiles.hpp>
#include <map>
#include <string>
#include <unordered_set>

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			class ChemfilesWriter
			{
			  public:
				void writeFile( const FilePath &, const Model::Molecule & );
				void writeBuffer( std::string &, const Model::Molecule & );
				void fillTrajectoryFrames( chemfiles::Trajectory &, Model::Molecule & ) const;

				bool isChainMerged( const Model::Chain & p_chain ) const;
				uint getNewResidueIndex( const Model::Residue & ) const;
				uint getNewAtomIndex( const Model::Atom & ) const;

			  private:
				void _writeTrajectory( chemfiles::Trajectory &, const Model::Molecule & );

				std::unordered_set<uint> _mergedChains		   = std::unordered_set<uint>();
				std::map<uint, uint>	 _mapNewResidueIndexes = std::map<uint, uint>();
				std::map<uint, uint>	 _mapNewAtomIndexes	   = std::map<uint, uint>();
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
