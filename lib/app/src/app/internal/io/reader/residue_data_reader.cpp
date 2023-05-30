#include "app/internal/io/reader/residue_data_reader.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/internal/io/filesystem.hpp"

namespace VTX::App::Internal::IO::Reader
{
	const ResidueData ResidueData::DEFAULT = ResidueData( "Unknown", std::vector<BondData>() );

	bool ResidueDataReader::readResidueData( const std::string & p_residueSymbol, ResidueData & p_residueData )
	{
		const FilePath filepath = App::Internal::IO::Filesystem::getResidueDataFilePath( p_residueSymbol );

		if ( !std::filesystem::exists( filepath ) )
		{
			return false;
		}

		std::string residueFileData = Util::Filesystem::readPath( filepath );

		std::stringstream stream = std::stringstream( residueFileData );

		_goToResidue( stream, p_residueSymbol );

		std::getline( stream, p_residueData.fullname, '\n' );
		std::string categoryStr;
		std::getline( stream, categoryStr, '\n' );

		p_residueData.category = VTX::Core::ChemDB::Category::TYPE( std::atoi( categoryStr.c_str() ) );

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
			bondData.bondOrder = VTX::Core::ChemDB::Bond::ORDER( std::stoi( orderStr ) );

			p_residueData.bondData.emplace_back( bondData );
		}

		return true;
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

} // namespace VTX::App::Internal::IO::Reader
