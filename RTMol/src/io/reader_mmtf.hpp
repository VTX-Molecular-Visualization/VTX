#ifndef __READER_MMTF_HPP__
#define __READER_MMTF_HPP__

#include "../model/molecule.hpp"
#include "path.hpp"

namespace IO
{
	class ReaderMMTF
	{
	  public:
		bool readFile( const Path &, Model::ModelMolecule & );
	};

} // namespace IO
#endif
