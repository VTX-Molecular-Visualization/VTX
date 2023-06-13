#ifndef __VTX_APP_MODEL_IO_READER_LIB_CHEMFILES__
#define __VTX_APP_MODEL_IO_READER_LIB_CHEMFILES__

// #include "app/core/io/reader/base_reader.hpp"
#include "app/model/chemistry/_fwd.hpp"
#include "app/model/io/molecule_metadata.hpp"
#include <string>
#include <util/types.hpp>
#include <utility>
#include <vector>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::App::Model::IO::Reader
{
	class MoleculeLoader /*: public App::Core::IO::Reader::BaseReader<App::Model::Chemistry::Molecule>*/
	{
	  public:
		MoleculeLoader() {};

		void readFile( const FilePath & p_path, App::Model::Chemistry::Molecule & p_molecule );
		void readBuffer( const std::string &, const FilePath &, App::Model::Chemistry::Molecule & );

		std::vector<Vec3f> readTrajectoryFrame( chemfiles::Trajectory & p_trajectory );
		void			   fillTrajectoryFrame( App::Model::Chemistry::Molecule & p_molecule,
												const size_t					  p_moleculeFrameIndex,
												const std::vector<Vec3f> &		  p_atomPositions );

	  private:
		void			  _readTrajectory( chemfiles::Trajectory &,
										   const FilePath &,
										   App::Model::Chemistry::Molecule &,
										   const bool p_recomputeBonds = false );
		const std::string _getFormat( const FilePath & );
		const bool		  _needToRecomputeBonds( const std::string & p_format ) const;

		void _readTrajectoryFrames( chemfiles::Trajectory & p_trajectory,
									const std::vector<std::pair<App::Model::Chemistry::Molecule *, size_t>> & p_targets,
									const size_t p_trajectoryFrameStart = 0 );

		void _warningCallback( const std::string & p_log );

		MoleculeMetadata _metaData = MoleculeMetadata();
	};
} // namespace VTX::App::Model::IO::Reader
#endif
