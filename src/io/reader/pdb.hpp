#ifndef __VTX_READER_PDB__
#define __VTX_READER_PDB__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class PDB : public BaseReader<Model::Molecule>
			{
			  public:
				virtual bool readFile( const Path &, Model::Molecule & ) override;
				virtual bool readBuffer( const std::string &, Model::Molecule & ) override;

			  private:
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
