#ifndef __VTX_READER_LIB_CHEMFILES__
#define __VTX_READER_LIB_CHEMFILES__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "model/molecule.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class LibChemfiles : public BaseReader<Model::Molecule>
			{
			  public:
				virtual void readFile( const Path &, Model::Molecule & ) override;
				virtual void readBuffer( const std::string &, const std::string &, Model::Molecule & ) override;

			  private:
				void prepareChemfiles() const;
				void readTrajectory( chemfiles::Trajectory &, Model::Molecule &, const std::string & ) const;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
