#ifndef __VTX_BASE_READER_LINE__
#define __VTX_BASE_READER_LINE__

#ifdef _MSC_VER
#pragma once
#endif

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

				virtual void readFile( const Path & p_path, T & p_data ) override final
				{
					// TODO: move stream opening in filesystem.hpp?
					std::ifstream file;
					std::string	  line;

					file.open( p_path );

					if ( !file.is_open() )
					{
						throw Exception::IOException( "Cannot open file" );
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
