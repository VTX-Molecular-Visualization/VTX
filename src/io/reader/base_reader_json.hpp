#ifndef __VTX_BASE_READER_JSON__
#define __VTX_BASE_READER_JSON__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "exception.hpp"
#include <nlohmann/json.hpp>

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
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
