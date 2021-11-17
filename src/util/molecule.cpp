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
	void show( Model::Chain & p_chain,
			   const bool	  p_show,
			   const bool	  p_showHierarchy,
			   const bool	  p_refreshMoleculeVisibility )
	{
		p_chain.setVisible( p_show );

		if ( p_show )
		{
			p_chain.getMoleculePtr()->setVisible( p_show );
		}

		Model::Molecule * const molecule = p_chain.getMoleculePtr();

		for ( uint residueID = p_chain.getIndexFirstResidue(); residueID <= p_chain.getIndexLastResidue(); residueID++ )
		{
			Model::Residue * const residue = molecule->getResidue( residueID );

			if ( residue == nullptr )
				continue;

			show( *residue, p_show, false, false );
		}

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void show( Model::Residue & p_residue,
			   const bool		p_show,
			   const bool		p_showHierarchy,
			   const bool		p_refreshMoleculeVisibility )
	{
		p_residue.setVisible( p_show );

		if ( p_show )
		{
			p_residue.getChainPtr()->setVisible( p_show );
			p_residue.getMoleculePtr()->setVisible( p_show );
		}

		Model::Molecule * const molecule = p_residue.getMoleculePtr();

		for ( uint atomID = p_residue.getIndexFirstAtom();
			  atomID < p_residue.getIndexFirstAtom() + p_residue.getAtomCount();
			  atomID++ )
		{
			Model::Atom * const atom = molecule->getAtom( atomID );

			if ( atom == nullptr )
				continue;

			show( *atom, p_show, false, false );
		}

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void show( Model::Atom & p_atom,
			   const bool	 p_show,
			   const bool	 p_showHierarchy,
			   const bool	 p_refreshMoleculeVisibility )
	{
		p_atom.setVisible( p_show );

		if ( p_show && p_showHierarchy )
		{
			p_atom.getResiduePtr()->setVisible( p_show );
			p_atom.getChainPtr()->setVisible( p_show );
			p_atom.getMoleculePtr()->setVisible( p_show );
		}

		if ( p_refreshMoleculeVisibility )
		{
			Model::Molecule * const molecule = p_atom.getMoleculePtr();

			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}

	void solo( Model::Molecule & p_molecule, const bool p_refreshMoleculeVisibility )
	{
		show( p_molecule, true, true );

	} // namespace VTX::Util::Molecule
	void solo( Model::Chain & p_chain, const bool p_refreshMoleculeVisibility )
	{
		Model::Molecule * const molecule = p_chain.getMoleculePtr();

		for ( Model::Chain * const chain : molecule->getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( chain != &p_chain )
			{
				show( *chain, false, false, false );
			}
			else
			{
				show( *chain, true, true, false );
			}
		}

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void solo( Model::Residue & p_residue, const bool p_refreshMoleculeVisibility )
	{
		Model::Chain * const	chainParent = p_residue.getChainPtr();
		Model::Molecule * const molecule	= chainParent->getMoleculePtr();

		for ( Model::Chain * const chain : molecule->getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( chain != chainParent )
			{
				show( *chain, false, false, false );
			}
			else
			{
				chainParent->setVisible( true );

				for ( uint residueID = chainParent->getIndexFirstResidue();
					  residueID <= chainParent->getIndexLastResidue();
					  residueID++ )
				{
					Model::Residue * const residue = molecule->getResidue( residueID );

					if ( residue == nullptr )
						continue;

					if ( residue == &p_residue )
					{
						show( *residue, true, true, false );
					}
					else
					{
						show( *residue, false, false, false );
					}
				}
			}
		}

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}

	} // namespace VTX::Util::Molecule
	void solo( Model::Atom & p_atom, const bool p_refreshMoleculeVisibility )
	{
		Model::Residue * const	residueParent = p_atom.getResiduePtr();
		Model::Chain * const	chainParent	  = residueParent->getChainPtr();
		Model::Molecule * const molecule	  = chainParent->getMoleculePtr();

		for ( Model::Chain * const chain : molecule->getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( chain != chainParent )
			{
				show( *chain, false, false, false );
			}
			else
			{
				chainParent->setVisible( true );

				for ( uint residueID = chainParent->getIndexFirstResidue();
					  residueID <= chainParent->getIndexLastResidue();
					  residueID++ )
				{
					Model::Residue * const residue = molecule->getResidue( residueID );

					if ( residue == nullptr )
						continue;

					if ( residue != residueParent )
					{
						show( *residue, false, false, false );
					}
					else
					{
						for ( uint atomID = residue->getIndexFirstAtom();
							  atomID < residue->getIndexFirstAtom() + residue->getAtomCount();
							  atomID++ )
						{
							Model::Atom * const atom = molecule->getAtom( atomID );

							if ( atom == nullptr )
								continue;

							atom->setVisible( atom == &p_atom );
						}
					}
				}
			}
		}

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
} // namespace VTX::Util::Molecule
