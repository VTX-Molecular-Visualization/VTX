#include "molecule.hpp"
#include "model/atom.hpp"
#include "model/bond.hpp"
#include "model/chain.hpp"
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

	bool recomputeBondOrdersFromFile( Model::Molecule & p_molecule )
	{
		return Util::BondGuessing::BondOrderGuessing::recomputeBondOrdersFromFile( p_molecule );
	}

	void show( Model::Molecule & p_molecule, const bool p_show, const bool p_refreshMoleculeVisibility )
	{
		p_molecule.setVisible( p_show );
		for ( Model::Chain * const chain : p_molecule.getChains() )
		{
			if ( chain != nullptr )
				chain->setVisible( p_show );
		}
		for ( Model::Residue * const residue : p_molecule.getResidues() )
		{
			if ( residue != nullptr )
				residue->setVisible( p_show );
		}
		for ( Model::Atom * const atom : p_molecule.getAtoms() )
		{
			if ( atom != nullptr )
				atom->setVisible( p_show );
		}

		if ( p_refreshMoleculeVisibility )
		{
			p_molecule.refreshVisibilities();
			p_molecule.computeRepresentationTargets();
		}
	}
	void show( Model::Chain & p_chain, const bool p_show, const bool p_refreshMoleculeVisibility )
	{
		p_chain.setVisible( p_show );

		Model::Molecule * const molecule = p_chain.getMoleculePtr();

		for ( uint residueID = p_chain.getIndexFirstResidue(); residueID <= p_chain.getIndexLastResidue(); residueID++ )
		{
			Model::Residue * const residue = molecule->getResidue( residueID );

			if ( residue == nullptr )
				continue;

			show( *residue, p_show, false );
		}

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void show( Model::Residue & p_residue, const bool p_show, const bool p_refreshMoleculeVisibility )
	{
		p_residue.setVisible( p_show );

		Model::Molecule * const molecule = p_residue.getMoleculePtr();

		for ( uint atomID = p_residue.getIndexFirstAtom();
			  atomID < p_residue.getIndexFirstAtom() + p_residue.getAtomCount();
			  atomID++ )
		{
			Model::Atom * const atom = molecule->getAtom( atomID );

			if ( atom == nullptr )
				continue;

			atom->setVisible( true );
		}

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}

} // namespace VTX::Util::Molecule
