#ifndef __VTX_APP_OLD_INTERNAL_IO_WRITER_WRITER_EXCEPTION__
#define __VTX_APP_OLD_INTERNAL_IO_WRITER_WRITER_EXCEPTION__

#include "app/old/core/io/writer/base_writer.hpp"

namespace VTX::App::Old::Internal::IO::Writer
{
	class Exception : public Core::IO::Writer::BaseWriter<std::exception>
	{
	  public:
		void writeFile( const FilePath & p_path, const std::exception & ) override;
	};
} // namespace VTX::App::Old::Internal::IO::Writer
#endif
