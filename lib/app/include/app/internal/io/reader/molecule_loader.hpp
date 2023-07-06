#ifndef __VTX_APP_INTERNAL_IO_MOLECULE_LOADER__
#define __VTX_APP_INTERNAL_IO_MOLECULE_LOADER__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include <io/core/reader/base_reader.hpp>
#include <io/reader/chemfiles.hpp>
#include <string>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::App::Internal::IO::Reader
{
	class MoleculeLoader : public VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::Molecule>
	{
	  public:
		MoleculeLoader() : VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::Molecule>() {};

		void readFile( const FilePath & p_path, App::Component::Chemistry::Molecule & p_molecule ) override;
		void readBuffer( const std::string &, const FilePath &, App::Component::Chemistry::Molecule & ) override;

		std::vector<Vec3f> readTrajectoryFrame( VTX::IO::Reader::Chemfiles & p_chemfilesReader );
		void			   fillTrajectoryFrame( App::Component::Chemistry::Molecule & p_molecule,
												const size_t						  p_moleculeFrameIndex,
												const std::vector<Vec3f> &			  p_atomPositions );

	  private:
		void			  _fillStructure( VTX::IO::Reader::Chemfiles & p_chemfilesReader,
										  App::Component::Chemistry::Molecule &,
										  const bool p_recomputeBonds = false );
		const std::string _getFormat( const FilePath & );
		const bool		  _needToRecomputeBonds( const std::string & p_format ) const;

		void _readTrajectoryFrames(
			VTX::IO::Reader::Chemfiles &												  p_chemfilesReader,
			const std::vector<std::pair<App::Component::Chemistry::Molecule *, size_t>> & p_targets,
			const size_t																  p_trajectoryFrameStart = 0 );

		void _warningCallback( const std::string & p_log );

		Component::IO::MoleculeMetadata _metaData = Component::IO::MoleculeMetadata();
	};
} // namespace VTX::App::Internal::IO::Reader
#endif
