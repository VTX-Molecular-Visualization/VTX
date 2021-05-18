#ifndef __VTX_READER_LIB_CHEMFILES__
#define __VTX_READER_LIB_CHEMFILES__

#ifdef _MSC_VER
#pragma once
#endif

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
		class Loader;
	}
	namespace IO::Reader
	{
		class LibChemfiles : public BaseReader<Model::Molecule>, ChemfilesIO
		{
		  public:
			LibChemfiles( Worker::Loader * const p_loader );

			void readFile( const FilePath &, Model::Molecule & ) override;
			void readBuffer( const std::string &, const FilePath &, Model::Molecule & ) override;
			void fillTrajectoryFrames( chemfiles::Trajectory &, Model::Molecule & ) const;

		  private:
			void _readTrajectory( chemfiles::Trajectory &, const FilePath &, Model::Molecule & ) const;
		};
	} // namespace IO::Reader
} // namespace VTX
#endif
