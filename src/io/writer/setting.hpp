#ifndef __VTX_WRITER_SETTING__
#define __VTX_WRITER_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_writer.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			class Setting : public BaseWriter<::VTX::Setting>
			{
			  public:
				virtual ~Setting() = default;

				virtual void writeFile( const Path & p_path, const ::VTX::Setting & p_data ) override;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
