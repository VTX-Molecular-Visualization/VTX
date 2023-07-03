#include "app/internal/util/molecule.hpp"
#include <sstream>

namespace VTX::App::Internal::Util::Molecule
{
	void loadResidueData( const std::string & p_residueSymbol )
	{
		VTX::IO::Reader::ResidueDataReader reader = IO::Reader::ResidueDataReader();
		VTX::IO::Reader::ResidueData	   residueData;

		if ( reader.readResidueData( p_residueSymbol, residueData ) )
			mapLoadedResidueData.emplace( p_residueSymbol, residueData );
		else
			mapLoadedResidueData.emplace( p_residueSymbol, IO::Reader::ResidueData::DEFAULT );
	}

	const std::string & getResidueFullName( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].fullname;
	}

	const std::vector<VTX::IO::Reader::BondData> & getResidueBonds( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].bondData;
	}

	VTX::Core::ChemDB::Category::TYPE getResidueCategory( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].category;
	}
} // namespace VTX::App::Internal::Util::Molecule
