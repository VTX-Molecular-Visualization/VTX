#ifndef __VTX_READER_LIB_CHEMFILES__
#define __VTX_READER_LIB_CHEMFILES__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
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
	namespace IO
	{
		namespace Reader
		{
			class LibChemfiles : public BaseReader<Model::Molecule>
			{
			  public:
				LibChemfiles( Worker::Loader * const p_loader ) : _loader( p_loader ) {}

				void readFile( const FilePath &, Model::Molecule & ) override;
				void readBuffer( const std::string &, const FilePath &, Model::Molecule & ) override;
				void fillTrajectoryFrames( chemfiles::Trajectory &, Model::Molecule & ) const;

			  private:
				Worker::Loader * const _loader;

				void _prepareChemfiles() const;
				void _readTrajectory( chemfiles::Trajectory &, const FilePath &, Model::Molecule & ) const;

				void _logError( const std::string & p_log ) const;
				void _logWarning( const std::string & p_log ) const;
				void _logInfo( const std::string & p_log ) const;
				void _logDebug( const std::string & p_log ) const;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
