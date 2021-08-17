#include "residue_data_reader.hpp"
#include "util/filesystem.hpp"

namespace VTX::IO::Reader
{
	void ResidueDataReader::readResidueData( const std::string & p_residueSymbol, ResidueData & p_residueData )
	{
		const FilePath filepath		   = Util::Filesystem::getResidueDataFilePath( p_residueSymbol );
		std::string	   residueFileData = Util::Filesystem::readPath( filepath );

		std::stringstream stream = std::stringstream( residueFileData );

		_goToResidue( stream, p_residueSymbol );

		std::getline( stream, p_residueData.fullname, '\n' );

		std::string line;
		while ( std::getline( stream, line ) )
		{
			if ( line.size() <= 0 || line[ 0 ] == '#' )
				break;

			std::stringstream streamLine = std::stringstream( line );

			BondData bondData = BondData();

			std::getline( streamLine, bondData.atom1, ',' );
			std::getline( streamLine, bondData.atom2, ',' );

			std::string orderStr;
			std::getline( streamLine, orderStr, ',' );
			bondData.bondOrder = Model::Bond::ORDER( std::stoi( orderStr ) );

			p_residueData.bondData.emplace_back( bondData );
		}
	}

	void ResidueDataReader::_goToResidue( std::stringstream & p_stream, const std::string & p_residueSymbol )
	{
		const std::string residueLine = "#" + p_residueSymbol;
		std::string		  line;
		while ( std::getline( p_stream, line ) )
		{
			if ( line == residueLine )
				break;
		}
	}

} // namespace VTX::IO::Reader
