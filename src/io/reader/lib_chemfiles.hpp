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

	namespace IO
	{
		namespace Reader
		{
			class LibChemfiles : public BaseReader<Model::Molecule>
			{
			  public:
				void readFile( const FilePath &, Model::Molecule & ) override;
				void readBuffer( const std::string &, const FilePath &, Model::Molecule & ) override;

			  private:
				void prepareChemfiles() const;
				void readTrajectory( chemfiles::Trajectory &, const FilePath &, Model::Molecule & ) const;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
