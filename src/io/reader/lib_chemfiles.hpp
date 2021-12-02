#ifndef __VTX_READER_LIB_CHEMFILES__
#define __VTX_READER_LIB_CHEMFILES__

#include "base_reader.hpp"
#include "io/chemfiles_io.hpp"
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

			void readFile( const IO::FilePath &, Model::Molecule & ) override;
			void readBuffer( const std::string &, const IO::FilePath &, Model::Molecule & ) override;
			bool readDynamic( const IO::FilePath &, std::vector<Model::Molecule *> p_potentialTargets );

			void fillTrajectoryFrames( chemfiles::Trajectory & p_trajectory,
									   Model::Molecule &,
									   const uint p_molFrameStart		 = 0,
									   const uint p_trajectoryFrameStart = 0 ) const;
			void LibChemfiles::fillTrajectoryFrame( const chemfiles::Frame & p_frame,
													Model::Molecule &		 p_molecule,
													const uint				 p_moleculeFrameIndex ) const;

		  private:
			void			  _readTrajectory( chemfiles::Trajectory &,
											   const IO::FilePath &,
											   Model::Molecule &,
											   const bool p_recomputeBonds = false ) const;
			const std::string _getFormat( const IO::FilePath & );
			const bool		  _needToRecomputeBonds( const std::string & p_format ) const;

			bool _tryApplyingDynamicOnTargets( chemfiles::Trajectory &		  p_dynamicTrajectory,
											   std::vector<Model::Molecule *> p_potentialTargets ) const;
		};
	} // namespace IO::Reader
} // namespace VTX
#endif
