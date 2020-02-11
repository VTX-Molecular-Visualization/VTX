#ifndef __VTX_READER_MMTF__
#define __VTX_READER_MMTF__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"

namespace VTX
{
	namespace IO
	{
		class ReaderMMTF : public BaseReader
		{
		  public:
			virtual bool readFile( const Path &, Model::Molecule & ) override;
		};

	} // namespace IO
} // namespace VTX
#endif
