#ifndef __VTX_APP_CORE_IO_READER_BASE_READER__
#define __VTX_APP_CORE_IO_READER_BASE_READER__

#include <util/exceptions.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::IO::Reader
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
} // namespace VTX::App::Core::IO::Reader
#endif
