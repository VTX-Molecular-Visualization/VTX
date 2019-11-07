#ifndef __VTX_BASE_READER__
#define __VTX_BASE_READER__

#include "../model/model_molecule.hpp"
#include "path.hpp"

namespace VTX
{
	namespace IO
	{
		class BaseReader
		{
		  public:
			virtual void readFile( const Path &, Model::ModelMolecule & ) = 0;
		};

	} // namespace IO
} // namespace VTX
#endif
