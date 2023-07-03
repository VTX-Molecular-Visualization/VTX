#include "app/old/internal/io/reader/psf.hpp"
#include "app/old/component/io/molecule_configuration.hpp"
#include <fstream>
#include <sstream>

namespace VTX::App::Old::Internal::IO::Reader
{
	void PSF::_readLine( const std::string & p_line, App::Old::Component::IO::MoleculeConfiguration & p_configuration )
	{
		if ( p_line.find( "WT", 0 ) != std::string::npos )
		{
			p_configuration.solventResidueSymbols.emplace( _readSymbol( p_line ) );
		}
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

} // namespace VTX::App::Old::Internal::IO::Reader
