#ifndef __VTX_IO_CORE_READER_BASE_READER__
#define __VTX_IO_CORE_READER_BASE_READER__

#include <string>
#include <util/exceptions.hpp>
#include <util/types.hpp>

namespace VTX::IO::Core::Reader
{
	template<typename T>
	class BaseReader
	{
	  public:
		virtual ~BaseReader() = default;

		virtual void readFile( const FilePath &, T & ) = 0;
		virtual void readBuffer( const std::string & p_buffer, const FilePath &, T & )
		{
			throw NotImplementedException();
		}
	};
} // namespace VTX::IO::Core::Reader
#endif
