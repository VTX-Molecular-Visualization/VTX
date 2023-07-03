#include "app/old/internal/io/reader/prm.hpp"
#include "app/old/component/io/molecule_configuration.hpp"
#include <fstream>
#include <sstream>

namespace VTX::App::Old::Internal::IO::Reader
{
	void PRM::_readLine( const std::string & p_line, App::Old::Component::IO::MoleculeConfiguration & p_configuration )
	{
		// Read only line starting with "atom".
		if ( p_line.rfind( "atom", 0 ) != 0 )
		{
			return;
		}
		else if ( p_line.find( "Water", 0 ) != std::string::npos )
		{
			p_configuration.solventAtomIds.emplace( _readId( p_line ) );
		}
		else if ( p_line.find( "Ion", 0 ) != std::string::npos )
		{
			p_configuration.ionAtomIds.emplace( _readId( p_line ) );
		}
	}

	uint PRM::_readId( const std::string & p_line ) const
	{
		std::istringstream iss;
		uint			   id;
		std::string		   str;

		iss.str( p_line );
		iss >> str;
		iss >> id;

		return id;
	}

} // namespace VTX::App::Old::Internal::IO::Reader
