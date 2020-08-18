#ifndef __VTX_READER_SETTING__
#define __VTX_READER_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class Setting : public BaseReader<::VTX::Setting>
			{
			  public:
				void readFile( const Path &, ::VTX::Setting & p_data ) override;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
