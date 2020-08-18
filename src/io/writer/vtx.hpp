#ifndef __VTX_WRITER_VTX__
#define __VTX_WRITER_VTX__

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
			class VTX : public BaseWriter<VTXApp>
			{
			  public:
				void writeFile( const Path & p_path, const VTXApp & p_data ) override;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
