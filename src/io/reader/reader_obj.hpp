#ifndef __VTX_READER_OBJ__
#define __VTX_READER_OBJ__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"

namespace VTX
{
	namespace IO
	{
		class ReaderOBJ : public BaseReader
		{
		  public:
			virtual bool readFile( const Path &, Model::Molecule & ) override;
		};

	} // namespace IO
} // namespace VTX
#endif
