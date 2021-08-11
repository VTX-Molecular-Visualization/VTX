#include "molecule.hpp"
#include "model/bond.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
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

	void recomputeBondOrdersWithFile( Model::Molecule & p_molecule )
	{
		int					   bondDataCurrentIndex = 0;
		const Model::Residue * previousResidue		= nullptr;
		for ( Model::Bond * const bond : p_molecule.getBonds() )
		{
			if ( bond->getOrder() != Model::Bond::ORDER::UNKNOWN )
				continue;

			const uint firstAtomIndex  = bond->getIndexFirstAtom();
			const uint secondAtomIndex = bond->getIndexSecondAtom();

			const Model::Residue * const firstAtomResidue  = p_molecule.getAtom( firstAtomIndex )->getResiduePtr();
			const Model::Residue * const secondAtomResidue = p_molecule.getAtom( secondAtomIndex )->getResiduePtr();

			if ( firstAtomResidue != secondAtomResidue )
				continue;

			const std::vector<IO::Reader::BondData> & bondsData = getResidueBonds( firstAtomResidue->getSymbolStr() );

			if ( previousResidue != firstAtomResidue )
			{
				bondDataCurrentIndex = 0;
				previousResidue		 = firstAtomResidue;
			}
			else
			{
				bondDataCurrentIndex++;
			}

			const std::string firstAtomName	 = p_molecule.getAtom( firstAtomIndex )->getName();
			const std::string secondAtomName = p_molecule.getAtom( secondAtomIndex )->getName();

			for ( bondDataCurrentIndex; bondDataCurrentIndex < bondsData.size(); bondDataCurrentIndex++ )
			{
				const IO::Reader::BondData & bondData = bondsData[ bondDataCurrentIndex ];

				if ( bondData.atom1 == firstAtomName && bondData.atom2 == secondAtomName )
				{
					bond->setOrder( bondData.bondOrder );
					break;
				}
			}
		}
	}

} // namespace VTX::Util::Molecule
