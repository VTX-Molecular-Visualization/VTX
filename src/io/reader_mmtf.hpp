#ifndef __VTX_READER_MMTF__
#define __VTX_READER_MMTF__

#include "base_reader.hpp"

namespace VTX
{
	namespace IO
	{
		class ReaderMMTF : public BaseReader
		{
		  public:
			ReaderMMTF() {}
			virtual void readFile( const Path &,
								   Model::ModelMolecule & ) override;
		};

	} // namespace IO
} // namespace VTX
#endif
