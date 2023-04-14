#ifndef __VTX_BASE_READER_LINE__
#define __VTX_BASE_READER_LINE__

#include "base_reader.hpp"
#include <fstream>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			template<typename T>
			class BaseReaderLine : public BaseReader<T>
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
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
