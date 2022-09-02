#ifndef __VTX_BASE_READER__
#define __VTX_BASE_READER__

#include "exception.hpp"
#include "io/filepath.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			template<typename T>
			class BaseReader
			{
			  public:
				virtual ~BaseReader() = default;

				virtual void readFile( const IO::FilePath &, T & ) = 0;
				virtual void readBuffer( const std::string & p_buffer, const IO::FilePath &, T & )
				{
					throw Exception::NotImplementedException();
				}
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
