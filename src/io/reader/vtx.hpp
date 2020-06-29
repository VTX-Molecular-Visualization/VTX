#ifndef __VTX_READER_VTX__
#define __VTX_READER_VTX__

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
			// Scene or app?
			class VTX : public BaseReaderJson<VTXApp>
			{
			  public:
				virtual ~VTX() = default;

				virtual void readFile( const Path &, VTXApp & ) override;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
