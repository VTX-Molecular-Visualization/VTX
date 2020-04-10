#ifndef __VTX_BASE_READER__
#define __VTX_BASE_READER__

#ifdef _MSC_VER
#pragma once
#endif

#include "exception.hpp"
#include "io/path.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			template<typename T>
			class BaseReader
			{
			  public:
				virtual void readFile( const Path &, T & )			= 0;
				virtual void readBuffer( const std::string &, T & ) = 0;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
