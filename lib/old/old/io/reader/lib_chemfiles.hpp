#ifndef __VTX_READER_LIB_CHEMFILES__
#define __VTX_READER_LIB_CHEMFILES__

#include "base_reader.hpp"
#include <util/types.hpp>
#include "io/chemfiles_io.hpp"
#include <utility>
#include <vector>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}
	namespace Worker
	{
		class BaseThread;
	}
	namespace IO::Reader
	{
		class LibChemfiles : public BaseReader<Model::Molecule>, ChemfilesIO
		{
		  public:
			LibChemfiles( const Worker::BaseThread * const p_loader );

			void readFile( const Util::FilePath &, Model::Molecule & ) override;
			void readBuffer( const std::string &, const Util::FilePath &, Model::Molecule & ) override;
			bool readDynamic( const Util::FilePath &, std::vector<Model::Molecule *> p_potentialTargets );

			std::vector<Vec3f> readTrajectoryFrame( chemfiles::Trajectory & p_trajectory ) const;
			void			   fillTrajectoryFrame( Model::Molecule &		   p_molecule,
													const uint				   p_moleculeFrameIndex,
													const std::vector<Vec3f> & p_atomPositions ) const;

		  private:
			void			  _readTrajectory( chemfiles::Trajectory &,
											   const Util::FilePath &,
											   Model::Molecule &,
											   const bool p_recomputeBonds = false ) const;
			const std::string _getFormat( const Util::FilePath & );
			const bool		  _needToRecomputeBonds( const std::string & p_format ) const;

			bool _tryApplyingDynamicOnTargets( chemfiles::Trajectory &				  p_dynamicTrajectory,
											   const std::vector<Model::Molecule *> & p_potentialTargets ) const;
			void _readTrajectoryFrames( chemfiles::Trajectory &									p_trajectory,
										const std::vector<std::pair<Model::Molecule *, uint>> & p_targets,
										const uint p_trajectoryFrameStart = 0 ) const;
		};
	} // namespace IO::Reader
} // namespace VTX
#endif
