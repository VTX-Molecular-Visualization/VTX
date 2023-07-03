#ifndef __VTX_APP_OLD_CORE_IO_WRITER_BASE_WRITER__
#define __VTX_APP_OLD_CORE_IO_WRITER_BASE_WRITER__

#include <util/exceptions.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Core::IO::Writer
{
	template<typename T>
	class BaseWriter
	{
	  public:
		virtual ~BaseWriter() = default;

		virtual void writeFile( const FilePath &, const T & ) = 0;
	};
} // namespace VTX::App::Old::Core::IO::Writer
#endif
