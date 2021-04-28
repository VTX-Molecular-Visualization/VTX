#ifndef __VTX_WRITE_CHEMFILES__
#define __VTX_WRITE_CHEMFILES__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include <chemfiles.hpp>
#include <string>

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			class ChemfilesWriter
			{
			  public:
				void writeFile( const FilePath &, const Model::Molecule & );
				void writeBuffer( std::string &, const Model::Molecule & );
				void fillTrajectoryFrames( chemfiles::Trajectory &, Model::Molecule & ) const;

			  private:
				void writeTrajectory( chemfiles::Trajectory &, const Model::Molecule & ) const;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
