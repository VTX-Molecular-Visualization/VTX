#ifndef __VTX_BASE_READER_JSON__
#define __VTX_BASE_READER_JSON__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			template<typename T>
			class BaseReaderJson : public BaseReader<T>
			{
			  public:
				virtual ~BaseReaderJson() = default;

				virtual void readFile( const Path & p_path, T & p_data ) override {}
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
