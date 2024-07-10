#include "io/reader/residue_data_reader.hpp"
#include "io/internal/filesystem.hpp"
#include "io/struct/bond_data.hpp"
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::IO::Reader
{
	bool ResidueDataReader::readResidueData(
		const FilePath &	  p_filepath,
		const std::string &	  p_residueSymbol,
		Struct::ResidueData & p_residueData
	)
	{
		if ( not std::filesystem::exists( p_filepath ) )
		{
			VTX_WARNING( "Residue data file not found: {}", p_filepath.string() );
			return false;
		}

		std::string residueFileData = VTX::Util::Filesystem::readPath( p_filepath );

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

			Struct::BondData bondData = Struct::BondData();

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

} // namespace VTX::IO::Reader
