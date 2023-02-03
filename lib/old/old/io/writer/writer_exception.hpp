#ifndef __VTX_WRITER_EXCEPTION__
#define __VTX_WRITER_EXCEPTION__

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
				void writeFile( const Util::FilePath & p_path, const std::exception & ) override;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
