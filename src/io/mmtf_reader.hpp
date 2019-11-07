#ifndef __VTX_MMTF_READER__
#define __VTX_MMTF_READER__

#include "base_reader.hpp"

namespace VTX
{
	namespace IO
	{
		class MMTFReader : public BaseReader
		{
		  public:
			virtual void readFile( const Path &,
								   Model::ModelMolecule & ) override;
		};

	} // namespace IO
} // namespace VTX
#endif
