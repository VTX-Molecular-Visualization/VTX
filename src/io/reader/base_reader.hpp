#ifndef __VTX_BASE_READER__
#define __VTX_BASE_READER__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "io/path.hpp"

namespace VTX
{
	namespace IO
	{
		class BaseReader
		{
		  public:
			virtual bool readFile( const Path &, Model::Molecule & ) = 0;
		};

	} // namespace IO
} // namespace VTX
#endif
