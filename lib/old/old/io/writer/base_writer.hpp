#ifndef __VTX_BASE_WRITER__
#define __VTX_BASE_WRITER__

#include "exception.hpp"
#include <util/filepath.hpp>

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

				virtual void writeFile( const Util::FilePath &, const T & ) = 0;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif