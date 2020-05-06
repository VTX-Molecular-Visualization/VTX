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
			void PSF::_readLine( const std::string & p_line, Model::Configuration::Molecule & p_configuration )
			{
				// TODO: are PSF files always like this ?
				if ( p_line.find( "WT", 0 ) != std::string::npos )
				{ p_configuration.solventResidueSymbols.emplace( _readSymbol( p_line ) ); }
				else if ( p_line.find( "ION", 0 ) != std::string::npos )
				{
					p_configuration.ionResidueSymbols.emplace( _readSymbol( p_line ) );
				}
			}

			std::string PSF::_readSymbol( const std::string & p_line ) const
			{
				std::istringstream iss;
				uint			   id;
				std::string		   str;

				iss.str( p_line );
				iss >> str;
				iss >> str;
				iss >> id;
				iss >> str;

				return str.substr( 0, 3 );
			}

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
