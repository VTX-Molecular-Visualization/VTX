#include "prm.hpp"
#include "util/logger.hpp"
#include <fstream>
#include <sstream>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			bool PRM::readFile( const Path & p_path, PRMFile & p_prm )
			{
				std::ifstream	   file;
				std::string		   line;
				std::istringstream iss;
				uint			   id;
				std::string		   str;

				file.open( p_path );

				if ( !file.is_open() ) { return false; }

				while ( getline( file, line ) )
				{
					// Read only line starting with "atom".
					if ( line.rfind( "atom", 0 ) != 0 ) { continue; }
					else if ( line.find( "Water", 0 ) != std::string::npos )
					{
						VTX_INFO( line );
						iss.clear();
						iss.str( line );
						iss >> str;
						iss >> id;
						p_prm.solventIds.push_back( id );
					}
					else if ( line.find( "Ion", 0 ) != std::string::npos )
					{
						VTX_INFO( line );
						iss.clear();
						iss.str( line );
						iss >> str;
						iss >> id;
						p_prm.ionIds.push_back( id );
					}
				}

				return true;
			}

			bool PRM::readBuffer( const std::string & p_buffer, PRMFile & p_prm ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
