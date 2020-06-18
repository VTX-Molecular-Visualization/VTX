#ifndef __VTX_BASE_WRITER_JSON__
#define __VTX_BASE_WRITER_JSON__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_writer.hpp"
#include <nlohmann/json.hpp>

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			template<typename T>
			class BaseWriterJson : public BaseWriter<T>
			{
			  public:
				virtual ~BaseWriterJson() = default;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
