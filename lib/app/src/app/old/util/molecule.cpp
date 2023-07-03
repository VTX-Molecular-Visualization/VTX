#include "app/old/util/molecule.hpp"
#include "app/old/component/chemistry/atom.hpp"
#include "app/old/component/chemistry/bond.hpp"
#include "app/old/component/chemistry/category.hpp"
#include "app/old/component/chemistry/chain.hpp"
#include "app/old/component/chemistry/residue.hpp"
#include "app/old/internal/algorithm/bond_order_guessing.hpp"
#include <sstream>

namespace VTX::Util::App::Old::Molecule
{
	void loadResidueData( const std::string & p_residueSymbol )
	{
		VTX::App::Old::Internal::IO::Reader::ResidueDataReader reader
			= VTX::App::Old::Internal::IO::Reader::ResidueDataReader();
		VTX::App::Old::Internal::IO::Reader::ResidueData residueData;

		if ( reader.readResidueData( p_residueSymbol, residueData ) )
			mapLoadedResidueData.emplace( p_residueSymbol, residueData );
		else
			mapLoadedResidueData.emplace( p_residueSymbol, VTX::App::Old::Internal::IO::Reader::ResidueData::DEFAULT );
	}

	const std::string & getResidueFullName( const std::string & p_residueSymbol )
	{
		if ( mapLoadedResidueData.find( p_residueSymbol ) == mapLoadedResidueData.end() )
			loadResidueData( p_residueSymbol );

		return mapLoadedResidueData[ p_residueSymbol ].fullname;
	}

	const std::vector<VTX::App::Old::Internal::IO::Reader::BondData> & getResidueBonds(
		const std::string & p_residueSymbol )
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

	void recomputeBondOrders( VTX::App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		VTX::App::Old::Internal::Algorithm::BondOrderGuessing::recomputeBondOrders( p_molecule );
	}

	bool recomputeBondOrdersFromFile( VTX::App::Old::Component::Chemistry::Molecule & p_molecule )
	{
		return VTX::App::Old::Internal::Algorithm::BondOrderGuessing::recomputeBondOrdersFromFile( p_molecule );
	}

	void show( VTX::App::Old::Component::Chemistry::Molecule & p_molecule,
			   const bool									   p_show,
			   const bool									   p_refreshMoleculeVisibility,
			   const bool									   p_notify )
	{
		p_molecule.setVisible( p_show, false );
		for ( VTX::App::Old::Component::Chemistry::Category * const category : p_molecule.getCategories() )
		{
			if ( category != nullptr )
				category->setVisible( p_show, false );
		}
		for ( VTX::App::Old::Component::Chemistry::Chain * const chain : p_molecule.getChains() )
		{
			if ( chain != nullptr )
				chain->setVisible( p_show, false );
		}
		for ( VTX::App::Old::Component::Chemistry::Residue * const residue : p_molecule.getResidues() )
		{
			if ( residue != nullptr )
				residue->setVisible( p_show, false );
		}
		for ( VTX::App::Old::Component::Chemistry::Atom * const atom : p_molecule.getAtoms() )
		{
			if ( atom != nullptr )
				atom->setVisible( p_show, false );
		}

		if ( p_notify )
			p_molecule.notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			p_molecule.refreshVisibilities();
			p_molecule.computeRepresentationTargets();
		}
	}
	void show( VTX::App::Old::Component::Chemistry::Category & p_category,
			   const bool									   p_show,
			   const bool									   p_showHierarchy,
			   const bool									   p_refreshMoleculeVisibility,
			   const bool									   p_notify )
	{
		p_category.setVisible( p_show, false );

		if ( p_showHierarchy && p_show )
		{
			p_category.getMoleculePtr()->setVisible( p_show, false );
		}

		VTX::App::Old::Component::Chemistry::Molecule * const molecule = p_category.getMoleculePtr();

		for ( const uint chainIndex : p_category.getChains() )
		{
			VTX::App::Old::Component::Chemistry::Chain * const chain = molecule->getChain( chainIndex );

			if ( chain == nullptr )
				continue;

			show( *chain, p_show, false, false, false );
		}

		if ( p_notify )
			molecule->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void show( VTX::App::Old::Component::Chemistry::Chain & p_chain,
			   const bool									p_show,
			   const bool									p_showHierarchy,
			   const bool									p_refreshMoleculeVisibility,
			   const bool									p_notify )
	{
		p_chain.setVisible( p_show, false );

		// Force visibility of parents when shown
		if ( p_showHierarchy && p_show )
		{
			p_chain.getMoleculePtr()->setVisible( p_show, false );
			p_chain.getMoleculePtr()->getCategoryFromChain( p_chain )->setVisible( p_show, false );
		}

		VTX::App::Old::Component::Chemistry::Molecule * const molecule = p_chain.getMoleculePtr();

		for ( uint residueID = p_chain.getIndexFirstResidue(); residueID <= p_chain.getIndexLastResidue(); residueID++ )
		{
			VTX::App::Old::Component::Chemistry::Residue * const residue = molecule->getResidue( residueID );

			if ( residue == nullptr )
				continue;

			show( *residue, p_show, false, false, false );
		}

		if ( p_notify )
			molecule->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void show( VTX::App::Old::Component::Chemistry::Residue & p_residue,
			   const bool									  p_show,
			   const bool									  p_showHierarchy,
			   const bool									  p_refreshMoleculeVisibility,
			   const bool									  p_notify )
	{
		p_residue.setVisible( p_show, false );

		if ( p_showHierarchy && p_show )
		{
			p_residue.getChainPtr()->setVisible( p_show, false );
			p_residue.getMoleculePtr()->getCategoryFromChain( *p_residue.getChainPtr() )->setVisible( p_show, false );
			p_residue.getMoleculePtr()->setVisible( p_show, false );
		}

		VTX::App::Old::Component::Chemistry::Molecule * const molecule = p_residue.getMoleculePtr();

		for ( uint atomID = p_residue.getIndexFirstAtom();
			  atomID < p_residue.getIndexFirstAtom() + p_residue.getAtomCount();
			  atomID++ )
		{
			VTX::App::Old::Component::Chemistry::Atom * const atom = molecule->getAtom( atomID );

			if ( atom == nullptr )
				continue;

			show( *atom, p_show, false, false, false );
		}

		if ( p_notify )
			molecule->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void show( VTX::App::Old::Component::Chemistry::Atom & p_atom,
			   const bool								   p_show,
			   const bool								   p_showHierarchy,
			   const bool								   p_refreshMoleculeVisibility,
			   const bool								   p_notify )
	{
		p_atom.setVisible( p_show, false );

		if ( p_showHierarchy && p_show )
		{
			p_atom.getResiduePtr()->setVisible( p_show, false );
			p_atom.getChainPtr()->setVisible( p_show, false );
			p_atom.getMoleculePtr()->getCategoryFromChain( *p_atom.getChainPtr() )->setVisible( p_show, false );
			p_atom.getMoleculePtr()->setVisible( p_show, false );
		}

		if ( p_notify )
			p_atom.getMoleculePtr()->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			VTX::App::Old::Component::Chemistry::Molecule * const molecule = p_atom.getMoleculePtr();

			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}

	void solo( VTX::App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_refreshMoleculeVisibility )
	{
		show( p_molecule, true, true, true );
	}

	void solo( VTX::App::Old::Component::Chemistry::Category & p_category, const bool p_refreshMoleculeVisibility )
	{
		VTX::App::Old::Component::Chemistry::Molecule * const molecule = p_category.getMoleculePtr();

		for ( VTX::App::Old::Component::Chemistry::Category * const category : molecule->getFilledCategories() )
		{
			if ( category != &p_category )
			{
				show( *category, false, false, false, false );
			}
			else
			{
				show( *category, true, true, false, false );
			}
		}

		molecule->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void soloCategories( VTX::App::Old::Component::Chemistry::Molecule &		p_moleculeParent,
						 const std::vector<VTX::Core::ChemDB::Category::TYPE> & p_categories,
						 const bool												p_refreshMoleculeVisibility )
	{
		p_moleculeParent.setVisible( true, false );

		std::vector<VTX::Core::ChemDB::Category::TYPE>::const_iterator itCategoryToSoloize	 = p_categories.cbegin();
		VTX::Core::ChemDB::Category::TYPE							   categoryEnumToSoloize = *itCategoryToSoloize;

		for ( VTX::App::Old::Component::Chemistry::Category * const category : p_moleculeParent.getFilledCategories() )
		{
			if ( category->getCategoryEnum() == categoryEnumToSoloize )
			{
				show( *category, true, false, false, false );
				itCategoryToSoloize++;

				categoryEnumToSoloize = itCategoryToSoloize == p_categories.cend()
											? VTX::Core::ChemDB::Category::TYPE::COUNT
											: *itCategoryToSoloize;
			}
			else
			{
				show( *category, false, false, false, false );
			}
		}

		p_moleculeParent.notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			p_moleculeParent.refreshVisibilities();
			p_moleculeParent.computeRepresentationTargets();
		}
	}

	void solo( VTX::App::Old::Component::Chemistry::Chain & p_chain, const bool p_refreshMoleculeVisibility )
	{
		VTX::App::Old::Component::Chemistry::Molecule * const molecule = p_chain.getMoleculePtr();

		for ( VTX::App::Old::Component::Chemistry::Chain * const chain : molecule->getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( chain != &p_chain )
			{
				show( *chain, false, false, false, false );
			}
			else
			{
				show( *chain, true, true, false, false );
			}
		}

		molecule->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void soloChains( VTX::App::Old::Component::Chemistry::Molecule & p_moleculeParent,
					 const std::vector<uint> &						 p_chainIndexes,
					 const bool										 p_refreshMoleculeVisibility )
	{
		p_moleculeParent.setVisible( true, false );

		std::vector<uint>::const_iterator itChainToSoloize = p_chainIndexes.cbegin();
		uint							  idChainToSoloize = *itChainToSoloize;

		for ( uint iChain = 0; iChain < p_moleculeParent.getChainCount(); iChain++ )
		{
			VTX::App::Old::Component::Chemistry::Chain * const chain = p_moleculeParent.getChain( iChain );
			if ( iChain == idChainToSoloize )
			{
				show( *chain, true, false, false, false );
				itChainToSoloize++;

				idChainToSoloize
					= itChainToSoloize == p_chainIndexes.cend() ? p_moleculeParent.getChainCount() : *itChainToSoloize;
			}
			else
			{
				if ( chain != nullptr )
					show( *chain, false, false, false, false );
			}
		}

		p_moleculeParent.notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			p_moleculeParent.refreshVisibilities();
			p_moleculeParent.computeRepresentationTargets();
		}
	}

	void solo( VTX::App::Old::Component::Chemistry::Residue & p_residue, const bool p_refreshMoleculeVisibility )
	{
		VTX::App::Old::Component::Chemistry::Chain * const	  chainParent = p_residue.getChainPtr();
		VTX::App::Old::Component::Chemistry::Molecule * const molecule	  = chainParent->getMoleculePtr();

		for ( VTX::App::Old::Component::Chemistry::Chain * const chain : molecule->getChains() )
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
					VTX::App::Old::Component::Chemistry::Residue * const residue = molecule->getResidue( residueID );

					if ( residue == nullptr )
						continue;

					if ( residue == &p_residue )
					{
						show( *residue, true, true, false, false );
					}
					else
					{
						show( *residue, false, false, false, false );
					}
				}
			}
		}

		molecule->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void soloResidues( VTX::App::Old::Component::Chemistry::Molecule & p_moleculeParent,
					   const std::vector<uint> &					   p_residueIndexes,
					   const bool									   p_refreshMoleculeVisibility )
	{
		std::vector<uint>::const_iterator itResidueToSoloize = p_residueIndexes.cbegin();
		uint							  idResidueToSoloize = *itResidueToSoloize;
		uint idParentChainToSoloize = p_moleculeParent.getResidue( idResidueToSoloize )->getChainPtr()->getIndex();

		std::set<VTX::App::Old::Component::Chemistry::Category *> categoryToHide
			= std::set<VTX::App::Old::Component::Chemistry::Category *>();
		for ( VTX::App::Old::Component::Chemistry::Category * category : p_moleculeParent.getCategories() )
			categoryToHide.emplace( category );

		p_moleculeParent.setVisible( true, false );

		for ( uint iChain = 0; iChain < p_moleculeParent.getChainCount(); iChain++ )
		{
			VTX::App::Old::Component::Chemistry::Chain * const	  chain = p_moleculeParent.getChain( iChain );
			VTX::App::Old::Component::Chemistry::Category * const category
				= p_moleculeParent.getCategoryFromChain( *chain );

			if ( iChain == idParentChainToSoloize )
			{
				category->setVisible( true, false );
				chain->setVisible( true, false );
				categoryToHide.erase( category );

				for ( uint iResidue = chain->getIndexFirstResidue(); iResidue <= chain->getIndexLastResidue();
					  iResidue++ )
				{
					VTX::App::Old::Component::Chemistry::Residue * const residue
						= p_moleculeParent.getResidue( iResidue );

					if ( iResidue == idResidueToSoloize )
					{
						show( *residue, true, false, false, false );
						itResidueToSoloize++;

						if ( itResidueToSoloize == p_residueIndexes.cend() )
						{
							idResidueToSoloize	   = p_moleculeParent.getResidueCount();
							idParentChainToSoloize = p_moleculeParent.getChainCount();
						}
						else
						{
							idResidueToSoloize = *itResidueToSoloize;
							idParentChainToSoloize
								= p_moleculeParent.getResidue( idResidueToSoloize )->getChainPtr()->getIndex();
						}
					}
					else
					{
						if ( residue != nullptr )
							show( *residue, false, false, false, false );
					}
				}
			}
			else
			{
				if ( chain != nullptr )
				{
					show( *chain, false, false, false, false );
				}
			}
		}

		for ( VTX::App::Old::Component::Chemistry::Category * category : categoryToHide )
			category->setVisible( false, false );

		p_moleculeParent.notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			p_moleculeParent.refreshVisibilities();
			p_moleculeParent.computeRepresentationTargets();
		}
	}

	void solo( VTX::App::Old::Component::Chemistry::Atom & p_atom, const bool p_refreshMoleculeVisibility )
	{
		VTX::App::Old::Component::Chemistry::Residue * const  residueParent = p_atom.getResiduePtr();
		VTX::App::Old::Component::Chemistry::Chain * const	  chainParent	= residueParent->getChainPtr();
		VTX::App::Old::Component::Chemistry::Molecule * const molecule		= chainParent->getMoleculePtr();

		for ( VTX::App::Old::Component::Chemistry::Chain * const chain : molecule->getChains() )
		{
			if ( chain == nullptr )
				continue;

			if ( chain != chainParent )
			{
				show( *chain, false, false, false, false );
			}
			else
			{
				chainParent->setVisible( true );

				for ( uint residueID = chainParent->getIndexFirstResidue();
					  residueID <= chainParent->getIndexLastResidue();
					  residueID++ )
				{
					VTX::App::Old::Component::Chemistry::Residue * const residue = molecule->getResidue( residueID );

					if ( residue == nullptr )
						continue;

					if ( residue != residueParent )
					{
						show( *residue, false, false, false, false );
					}
					else
					{
						for ( uint atomID = residue->getIndexFirstAtom();
							  atomID < residue->getIndexFirstAtom() + residue->getAtomCount();
							  atomID++ )
						{
							VTX::App::Old::Component::Chemistry::Atom * const atom = molecule->getAtom( atomID );

							if ( atom == nullptr )
								continue;

							atom->setVisible( atom == &p_atom, false );
						}
					}
				}
			}
		}

		molecule->notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			molecule->refreshVisibilities();
			molecule->computeRepresentationTargets();
		}
	}
	void soloAtoms( VTX::App::Old::Component::Chemistry::Molecule & p_moleculeParent,
					const std::vector<uint> &						p_atomIndexes,
					const bool										p_refreshMoleculeVisibility )
	{
		std::vector<uint>::const_iterator itAtomToSoloize = p_atomIndexes.cbegin();
		uint							  idAtomToSoloize = *itAtomToSoloize;
		uint idParentResidueToSoloize = p_moleculeParent.getAtom( idAtomToSoloize )->getResiduePtr()->getIndex();
		uint idParentChainToSoloize	  = p_moleculeParent.getAtom( idAtomToSoloize )->getChainPtr()->getIndex();

		p_moleculeParent.setVisible( true, false );

		for ( uint iChain = 0; iChain < p_moleculeParent.getChainCount(); iChain++ )
		{
			VTX::App::Old::Component::Chemistry::Chain * const chain = p_moleculeParent.getChain( iChain );

			if ( iChain == idParentChainToSoloize )
			{
				chain->setVisible( true, false );
				for ( uint iResidue = chain->getIndexFirstResidue(); iResidue <= chain->getIndexLastResidue();
					  iResidue++ )
				{
					VTX::App::Old::Component::Chemistry::Residue * const residue
						= p_moleculeParent.getResidue( iResidue );

					if ( iResidue == idParentResidueToSoloize )
					{
						residue->setVisible( true, false );
						for ( uint iAtom = residue->getIndexFirstAtom();
							  iAtom < residue->getIndexFirstAtom() + residue->getAtomCount();
							  iAtom++ )
						{
							VTX::App::Old::Component::Chemistry::Atom * atom = p_moleculeParent.getAtom( iAtom );
							if ( iAtom == idAtomToSoloize )
							{
								show( *atom, true, false, false, false );
								itAtomToSoloize++;

								if ( itAtomToSoloize == p_atomIndexes.cend() )
								{
									idAtomToSoloize			 = p_moleculeParent.getAtomCount();
									idParentResidueToSoloize = p_moleculeParent.getResidueCount();
									idParentChainToSoloize	 = p_moleculeParent.getChainCount();
								}
								else
								{
									idAtomToSoloize = *itAtomToSoloize;
									idParentResidueToSoloize
										= p_moleculeParent.getAtom( idAtomToSoloize )->getResiduePtr()->getIndex();
									idParentChainToSoloize
										= p_moleculeParent.getAtom( idAtomToSoloize )->getChainPtr()->getIndex();
								}
							}
							else
							{
								if ( atom != nullptr )
									show( *atom, false, false, false, false );
							}
						}
					}
					else
					{
						if ( residue != nullptr )
							show( *residue, false, false, false, false );
					}
				}
			}
			else
			{
				if ( chain != nullptr )
					show( *chain, false, false, false, false );
			}
		}

		p_moleculeParent.notifyVisibilityChange();

		if ( p_refreshMoleculeVisibility )
		{
			p_moleculeParent.refreshVisibilities();
			p_moleculeParent.computeRepresentationTargets();
		}
	}
} // namespace VTX::Util::App::Old::Molecule
