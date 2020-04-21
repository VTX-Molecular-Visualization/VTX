#include "psf.hpp"
#include "util/logger.hpp"
#include <fstream>
#include <sstream>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void PSF::readFile( const Path & p_path, PSFFile & p_psf )
			{
				std::ifstream	   file;
				std::string		   line;
				std::istringstream iss;
				uint			   id;
				std::string		   str;

				file.open( p_path );

				if ( !file.is_open() ) { throw Exception::IOException( "Cannot open file" ); }

				// TODO: are PSF files always like this ?
				while ( getline( file, line ) )
				{
					if ( line.find( "WT", 0 ) != std::string::npos )
					{
						// VTX_INFO( line );
						iss.clear();
						iss.str( line );
						iss >> str;
						iss >> str;
						iss >> id;
						iss >> str;
						str = str.substr( 0, 3 );
						p_psf.solventResidueSymbols.emplace( str );
					}
					else if ( line.find( "ION", 0 ) != std::string::npos )
					{
						// VTX_INFO( line );
						iss.clear();
						iss.str( line );
						iss >> str;
						iss >> str;
						iss >> id;
						iss >> str;
						str = str.substr( 0, 3 );
						p_psf.ionResidueSymbols.emplace( str );
					}
				}
				VTX_INFO( "Solvents are " );
				for ( const std::string & s : p_psf.solventResidueSymbols )
				{
					VTX_INFO( " - " + s );
				}
				VTX_INFO( "Ions are:" );
				for ( const std::string & s : p_psf.ionResidueSymbols )
				{
					VTX_INFO( " - " + s );
				}
			}

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
