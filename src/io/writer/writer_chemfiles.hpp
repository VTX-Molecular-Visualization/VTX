#ifndef __VTX_WRITE_CHEMFILES__
#define __VTX_WRITE_CHEMFILES__

#include "base_writer.hpp"
#include "io/chemfiles_io.hpp"
#include "model/molecule.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <string>
#include <unordered_set>
#include <vector>

namespace VTX::IO::Writer
{
	class ChemfilesWriter : BaseWriter<Model::Molecule>, ChemfilesIO
	{
	  public:
		ChemfilesWriter() : ChemfilesIO() {};
		ChemfilesWriter( const Worker::BaseThread * const p_thread ) : ChemfilesIO( p_thread ) {}
		ChemfilesWriter( const Worker::BaseWorker * const p_worker ) : ChemfilesIO( p_worker ) {}

		void writeFile( const IO::FilePath &, const Model::Molecule & );
		void writeBuffer( std::string &, const Model::Molecule &, const std::string & p_format = "PDB" );
		void fillTrajectoryFrames( chemfiles::Trajectory &, Model::Molecule & ) const;

		uint getNewChainIndex( const Model::Chain & ) const;
		uint getNewResidueIndex( const Model::Residue & ) const;
		uint getNewAtomIndex( const Model::Atom & ) const;

	  private:
		void _writeTrajectory( chemfiles::Trajectory &, const Model::Molecule & );

		std::vector<uint> _vecNewChainIndexes	= std::vector<uint>();
		std::vector<uint> _vecNewResidueIndexes = std::vector<uint>();
		std::vector<uint> _vecNewAtomIndexes	= std::vector<uint>();
	};
} // namespace VTX::IO::Writer
#endif
