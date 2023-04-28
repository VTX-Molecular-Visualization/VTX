#ifndef __VTX_APP_CORE_IO_READER_BASE_READER_LINE__
#define __VTX_APP_CORE_IO_READER_BASE_READER_LINE__

#include "app/core/io/reader/base_reader.hpp"
#include <fstream>

namespace VTX::App::Core::IO::Reader
{
	template<typename T>
	class BaseReaderLine : public Core::IO::Reader::BaseReader<T>
	{
	  public:
		virtual ~BaseReaderLine() = default;

		virtual void readFile( const FilePath & p_path, T & p_data ) override final
		{
			std::ifstream file;
			std::string	  line;

			file.open( p_path );

			if ( !file.is_open() )
			{
				throw IOException( "Cannot open file" );
			}

			while ( getline( file, line ) )
			{
				_readLine( line, p_data );
			}
		}

	  protected:
		virtual void _readLine( const std::string &, T & ) = 0;
	};
} // namespace VTX::App::Core::IO::Reader
#endif
