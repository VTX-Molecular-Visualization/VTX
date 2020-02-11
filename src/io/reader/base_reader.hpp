#ifndef __VTX_BASE_READER__
#define __VTX_BASE_READER__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/path.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class BaseReader
			{
			  public:
				virtual bool readFile( const Path &, Model::Molecule & ) = 0;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
