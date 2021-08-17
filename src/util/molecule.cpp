#include "molecule.hpp"
#include "model/bond.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "util/bond_guessing/bond_order_guessing.hpp"
#include <sstream>

namespace VTX::Util::Molecule
{
	void loadResidueData( const std::string & p_residueSymbol )
	{
		IO::Reader::ResidueDataReader reader = IO::Reader::ResidueDataReader();
		IO::Reader::ResidueData		  residueData;
		reader.readResidueData( p_residueSymbol, residueData );

		mapLoadedResidueData.emplace( p_residueSymbol, residueData );
	}

	const std::string & getResidueFullName( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].fullname;
	}

	const std::vector<IO::Reader::BondData> & getResidueBonds( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].bondData;
	}

	void recomputeBondOrders( Model::Molecule & p_molecule )
	{
		Util::BondGuessing::BondOrderGuessing::recomputeBondOrders( p_molecule );
	}

	void recomputeBondOrdersFromFile( Model::Molecule & p_molecule )
	{
		Util::BondGuessing::BondOrderGuessing::recomputeBondOrdersFromFile( p_molecule );
	}

} // namespace VTX::Util::Molecule
