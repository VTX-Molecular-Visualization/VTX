#ifndef __VTX_APP_INTERNAL_IO_READER_LIB_CHEMFILES__
#define __VTX_APP_INTERNAL_IO_READER_LIB_CHEMFILES__

#include "app/component/chemistry/_fwd.hpp"
#include "app/core/io/reader/base_reader.hpp"
#include "app/internal/io/chemfiles_io.hpp"
#include <util/types.hpp>
#include <utility>
#include <vector>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::App::Internal::IO::Reader
{
	class LibChemfiles : public Core::IO::Reader::BaseReader<App::Component::Chemistry::Molecule>, ChemfilesIO
	{
	  public:
		LibChemfiles( const VTX::App::Core::Worker::BaseThread * const p_loader );

		void readFile( const FilePath &, App::Component::Chemistry::Molecule & ) override;
		void readBuffer( const std::string &, const FilePath &, App::Component::Chemistry::Molecule & ) override;
		bool readDynamic( const FilePath &, std::vector<App::Component::Chemistry::Molecule *> p_potentialTargets );

		std::vector<Vec3f> readTrajectoryFrame( chemfiles::Trajectory & p_trajectory ) const;
		void			   fillTrajectoryFrame( App::Component::Chemistry::Molecule & p_molecule,
												const uint							  p_moleculeFrameIndex,
												const std::vector<Vec3f> &			  p_atomPositions ) const;

	  private:
		void			  _readTrajectory( chemfiles::Trajectory &,
										   const FilePath &,
										   App::Component::Chemistry::Molecule &,
										   const bool p_recomputeBonds = false ) const;
		const std::string _getFormat( const FilePath & );
		const bool		  _needToRecomputeBonds( const std::string & p_format ) const;

		bool _tryApplyingDynamicOnTargets(
			chemfiles::Trajectory &									   p_dynamicTrajectory,
			const std::vector<App::Component::Chemistry::Molecule *> & p_potentialTargets ) const;
		void _readTrajectoryFrames(
			chemfiles::Trajectory &														p_trajectory,
			const std::vector<std::pair<App::Component::Chemistry::Molecule *, uint>> & p_targets,
			const uint p_trajectoryFrameStart = 0 ) const;
	};
} // namespace VTX::App::Internal::IO::Reader
#endif
