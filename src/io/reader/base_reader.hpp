#ifndef __VTX_BASE_READER__
#define __VTX_BASE_READER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "exception.hpp"

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
				virtual ~BaseReader() = default;

				virtual void readFile( const Path &, T & ) = 0;
				virtual void readBuffer( const std::string & p_buffer, const Path &, T & )
				{
					throw Exception::NotImplementedException();
				}
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
