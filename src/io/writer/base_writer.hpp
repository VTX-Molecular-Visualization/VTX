#ifndef __VTX_BASE_WRITER__
#define __VTX_BASE_WRITER__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/path.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			class BaseWriter
			{
			  public:
				virtual bool writeFile( const Path & ) = 0;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
