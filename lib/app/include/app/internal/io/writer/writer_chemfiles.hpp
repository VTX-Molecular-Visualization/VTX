#ifndef __VTX_APP_INTERNAL_IO_WRITER_WRITER_CHEMFILES__
#define __VTX_APP_INTERNAL_IO_WRITER_WRITER_CHEMFILES__

#include "app/component/chemistry/molecule.hpp"
#include "app/core/io/writer/base_writer.hpp"
#include "app/internal/io/chemfiles_io.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <string>
#include <unordered_set>
#include <vector>

namespace VTX::App::Internal::IO::Writer
{
	class ChemfilesWriter : Core::IO::Writer::BaseWriter<App::Component::Chemistry::Molecule>, ChemfilesIO
	{
	  public:
		ChemfilesWriter() : ChemfilesIO() {};
		ChemfilesWriter( const VTX::App::Core::Worker::BaseThread * const p_thread ) : ChemfilesIO( p_thread ) {}
		ChemfilesWriter( const VTX::App::Core::Worker::BaseWorker * const p_worker ) : ChemfilesIO( p_worker ) {}

		void writeFile( const FilePath &, const App::Component::Chemistry::Molecule & );
		void writeBuffer( std::string &,
						  const App::Component::Chemistry::Molecule &,
						  const std::string & p_format = "PDB" );
		void fillTrajectoryFrames( chemfiles::Trajectory &, App::Component::Chemistry::Molecule & ) const;

		uint getNewChainIndex( const App::Component::Chemistry::Chain & ) const;
		uint getNewResidueIndex( const App::Component::Chemistry::Residue & ) const;
		uint getNewAtomIndex( const App::Component::Chemistry::Atom & ) const;

	  private:
		void _writeTrajectory( chemfiles::Trajectory &, const App::Component::Chemistry::Molecule & );

		std::vector<uint> _vecNewChainIndexes	= std::vector<uint>();
		std::vector<uint> _vecNewResidueIndexes = std::vector<uint>();
		std::vector<uint> _vecNewAtomIndexes	= std::vector<uint>();
	};
} // namespace VTX::App::Internal::IO::Writer
#endif
