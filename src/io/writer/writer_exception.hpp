#ifndef __VTX_WRITER_EXCEPTION__
#define __VTX_WRITER_EXCEPTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_writer.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			class Exception : public BaseWriter<std::exception>
			{
			  public:
				void writeFile( const Path & p_path, const std::exception & ) override;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
