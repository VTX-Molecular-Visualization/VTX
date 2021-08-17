#ifndef __VTX_BASE_WRITER__
#define __VTX_BASE_WRITER__

#include "exception.hpp"
#include "io/filepath.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			template<typename T>
			class BaseWriter
			{
			  public:
				virtual ~BaseWriter() = default;

				virtual void writeFile( const IO::FilePath &, const T & ) = 0;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
