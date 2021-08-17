#ifndef __VTX_READER_LIB_CHEMFILES__
#define __VTX_READER_LIB_CHEMFILES__

#include "base_reader.hpp"
#include "io/chemfiles_io.hpp"
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
			void fillTrajectoryFrames( chemfiles::Trajectory &, Model::Molecule & ) const;

		  private:
			void _readTrajectory( chemfiles::Trajectory &,
								  const IO::FilePath &,
								  Model::Molecule &,
								  const bool p_recomputeBonds = false ) const;
			const std::string _getFormat( const IO::FilePath & );
			const bool		  _needToRecomputeBonds( const std::string & p_format ) const;
		};
	} // namespace IO::Reader
} // namespace VTX
#endif
