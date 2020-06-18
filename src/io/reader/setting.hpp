#ifndef __VTX_READER_SETTING__
#define __VTX_READER_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader_json.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class Setting : public BaseReaderJson<VTX::Setting>
			{
			  public:
				virtual ~Setting() = default;

				virtual void readFile( const Path &, VTX::Setting & p_data ) override;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
