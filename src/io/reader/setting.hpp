#ifndef __VTX_READER_SETTING__
#define __VTX_READER_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader_json.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class Setting : public BaseReaderJson<Setting>
			{
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
