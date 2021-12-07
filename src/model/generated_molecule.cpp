#include "generated_molecule.hpp"
#include "atom.hpp"
#include "chain.hpp"
#include "id.hpp"
#include "model/bond.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "residue.hpp"
#include "selection.hpp"
#include "selection/selection_manager.hpp"
#include "tool/chrono.hpp"
#include <map>

namespace VTX::Model
{
	GeneratedMolecule::GeneratedMolecule() : Model::Molecule( ID::Model::MODEL_GENERATED_MOLECULE ) {}

	void GeneratedMolecule::copyFromSelection( const Model::Selection & p_selection, const Model::ID & p_moleculeID )
	{
		Tool::Chrono chrono;
		chrono.start();

		const Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( p_moleculeID );
		const Model::Selection::MapChainIds & moleculeSelectionData = p_selection.getMoleculesMap().at( p_moleculeID );

		_copyMoleculeData( molecule, "Copy of " );

		std::map<const uint, const uint> mapAtomIds = std::map<const uint, const uint>();

		for ( const std::pair<const ID, const Model::Selection::MapResidueIds> & chainData : moleculeSelectionData )
		{
			const Model::Chain & chain			= *molecule.getChain( chainData.first );
			Model::Chain &		 generatedChain = addChain();

			_copyChainData( generatedChain, chain );

			generatedChain.setResidueCount( uint( chainData.second.size() ) );

			for ( const std::pair<const ID, const Model::Selection::VecAtomIds> & residueData : chainData.second )
			{
				const Model::Residue & residue			= *molecule.getResidue( residueData.first );
				Model::Residue &	   generatedResidue = addResidue();
				_copyResidueData( generatedResidue, residue, &generatedChain );
				generatedResidue.setAtomCount( uint( residueData.second.size() ) );

				for ( const ID & atomId : residueData.second )
				{
					const Model::Atom & atom = *molecule.getAtom( atomId );

					Model::Atom & generatedAtom = addAtom();
					generatedAtom.setResiduePtr( &generatedResidue );

					_copyAtomData( generatedAtom, atom, &generatedResidue );

					mapAtomIds.emplace( atom.getIndex(), generatedAtom.getIndex() );

					// Copy atom position for each frame
					for ( int i = 0; i < getAtomPositionFrames().size(); i++ )
						getAtomPositionFrame( i ).emplace_back( molecule.getAtomPositionFrame( i )[ atom.getIndex() ] );
				}
			}
		}

		_computeBonds( molecule, mapAtomIds );

		getBufferAtomVisibilities().resize( getAtomCount(), 1u );
		getBufferAtomSelections().resize( getAtomCount(), 0u );

		getBufferAtomRadius().shrink_to_fit();
		getBufferAtomIds().shrink_to_fit();
		for ( AtomPositionsFrame & atomPositions : getAtomPositionFrames() )
			atomPositions.shrink_to_fit();

		chrono.stop();
		VTX_INFO( "Copy done in : " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void GeneratedMolecule::copyFromMolecule( const Model::Molecule & p_molecule )
	{
		Tool::Chrono chrono;
		chrono.start();

		std::map<const uint, const uint> mapAtomIds = std::map<const uint, const uint>();
		_copyFullMolecule( p_molecule, mapAtomIds );

		_computeBonds( p_molecule, mapAtomIds );
		_validateBuffers();

		chrono.stop();
		VTX_INFO( "Copy done in : " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
	void GeneratedMolecule::copyFromChain( const Model::Chain & p_chain )
	{
		Tool::Chrono chrono;
		chrono.start();

		const Model::Molecule & molecule = *p_chain.getMoleculePtr();
		_copyMoleculeData( molecule, "Copy of " );

		std::map<const uint, const uint> mapAtomIds = std::map<const uint, const uint>();
		_copyFullChain( p_chain, molecule, mapAtomIds );

		_computeBonds( molecule, mapAtomIds );
		_validateBuffers();

		chrono.stop();
		VTX_INFO( "Copy done in : " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
	void GeneratedMolecule::copyFromResidue( const Model::Residue & p_residue )
	{
		Tool::Chrono chrono;
		chrono.start();

		const Model::Molecule & molecule = *p_residue.getMoleculePtr();
		_copyMoleculeData( molecule, "Copy of " );

		const Model::Chain & chain			= *p_residue.getChainPtr();
		Model::Chain &		 generatedChain = addChain();
		_copyChainData( generatedChain, chain );
		generatedChain.setResidueCount( 1 );

		std::map<const uint, const uint> mapAtomIds = std::map<const uint, const uint>();
		_copyFullResidue( p_residue, molecule, &generatedChain, mapAtomIds );

		_computeBonds( molecule, mapAtomIds );
		_validateBuffers();

		chrono.stop();
		VTX_INFO( "Copy done in : " + std::to_string( chrono.elapsedTime() ) + "s" );
	}
	void GeneratedMolecule::copyFromAtom( const Model::Atom & p_atom )
	{
		Tool::Chrono chrono;
		chrono.start();

		const Model::Molecule & molecule = *p_atom.getMoleculePtr();
		_copyMoleculeData( molecule, "Copy of " );

		const Model::Chain & chain			= *p_atom.getChainPtr();
		Model::Chain &		 generatedChain = addChain();
		_copyChainData( generatedChain, chain );
		generatedChain.setResidueCount( 1 );

		const Model::Residue & residue			= *p_atom.getResiduePtr();
		Model::Residue &	   generatedResidue = addResidue();
		_copyResidueData( generatedResidue, residue, &generatedChain );
		generatedResidue.setAtomCount( 1 );

		_copyAtom( p_atom, molecule, &generatedResidue );

		_validateBuffers();

		chrono.stop();
		VTX_INFO( "Copy done in : " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void GeneratedMolecule::_copyFullMolecule( const Model::Molecule &			  p_moleculeSource,
											   std::map<const uint, const uint> & p_mapAtomIds )
	{
		_copyMoleculeData( p_moleculeSource, "Copy of " );

		for ( uint i = 0; i < p_moleculeSource.getChainCount(); i++ )
		{
			const Model::Chain * const chain = p_moleculeSource.getChain( i );

			if ( chain == nullptr )
				continue;

			_copyFullChain( *chain, p_moleculeSource, p_mapAtomIds );
		}
	}
	Model::Chain & GeneratedMolecule::_copyFullChain( const Model::Chain &				 p_source,
													  const Model::Molecule &			 p_moleculeSource,
													  std::map<const uint, const uint> & p_mapAtomIds )
	{
		Model::Chain & generatedChain = addChain();
		_copyChainData( generatedChain, p_source );

		const uint firstIndex = p_source.getIndexFirstResidue();
		const uint lastIndex  = p_source.getIndexLastResidue();
		uint	   atomCount  = 0;

		uint residueCount = 0;

		for ( uint i = firstIndex; i <= lastIndex; i++ )
		{
			const Model::Residue * const residue = p_moleculeSource.getResidue( i );

			if ( residue == nullptr )
				continue;

			_copyFullResidue( *residue, p_moleculeSource, &generatedChain, p_mapAtomIds );
			residueCount++;
		}

		generatedChain.setResidueCount( residueCount );
		return generatedChain;
	}

	Model::Residue & GeneratedMolecule::_copyFullResidue( const Model::Residue &			 p_source,
														  const Model::Molecule &			 p_moleculeSource,
														  Model::Chain * const				 p_parent,
														  std::map<const uint, const uint> & p_mapAtomIds )
	{
		Model::Residue & generatedResidue = addResidue();
		_copyResidueData( generatedResidue, p_source, p_parent );

		const uint firstIndex = p_source.getIndexFirstAtom();
		const uint lastIndex  = p_source.getIndexFirstAtom() + p_source.getAtomCount() - 1;
		uint	   atomCount  = 0;

		for ( uint i = firstIndex; i <= lastIndex; i++ )
		{
			const Model::Atom * const atom = p_moleculeSource.getAtom( i );

			if ( atom == nullptr )
				continue;

			Model::Atom & generatedAtom = _copyAtom( *atom, p_moleculeSource, &generatedResidue );
			p_mapAtomIds.emplace( atom->getIndex(), generatedAtom.getIndex() );

			atomCount++;
		}

		generatedResidue.setAtomCount( atomCount );

		return generatedResidue;
	}
	Model::Atom & GeneratedMolecule::_copyAtom( const Model::Atom &		p_source,
												const Model::Molecule & p_moleculeSource,
												Model::Residue * const	p_parent )
	{
		Model::Atom & generatedAtom = addAtom();
		generatedAtom.setResiduePtr( p_parent );
		_copyAtomData( generatedAtom, p_source, p_parent );

		// Copy atom position for each frame
		for ( int i = 0; i < getAtomPositionFrames().size(); i++ )
		{
			getAtomPositionFrame( i ).emplace_back( p_moleculeSource.getAtomPositionFrame( i )[ p_source.getIndex() ] );
		}

		return generatedAtom;
	}

	void GeneratedMolecule::_computeBonds( const Model::Molecule &			  p_source,
										   std::map<const uint, const uint> & p_mapAtomIds )
	{
		if ( p_source.getBondCount() <= 0 )
			return;

		// Bonds
		getBufferBonds().reserve( p_source.getBufferBonds().size() );

		uint				indexFirstValidBond = 0;
		const Model::Bond * bondPtr				= p_source.getBonds()[ indexFirstValidBond ];

		while ( indexFirstValidBond < p_source.getBondCount() && bondPtr == nullptr )
		{
			indexFirstValidBond++;
			bondPtr = p_source.getBonds()[ indexFirstValidBond ];
		}

		if ( bondPtr == nullptr )
			return;

		Model::Residue * previousResidue	  = getAtom( bondPtr->getIndexFirstAtom() )->getResiduePtr();
		uint			 previousResidueIndex = previousResidue->getIndex();
		uint			 counter			  = 0;

		for ( uint i = indexFirstValidBond; i < p_source.getBondCount(); i++ )
		{
			bondPtr = p_source.getBonds()[ i ];

			if ( bondPtr == nullptr )
				continue;

			const Model::Bond & bond = *bondPtr;

			if ( p_mapAtomIds.find( bond.getIndexFirstAtom() ) != p_mapAtomIds.end()
				 && p_mapAtomIds.find( bond.getIndexSecondAtom() ) != p_mapAtomIds.end() )
			{
				const uint indexFirstAtom  = p_mapAtomIds[ bond.getIndexFirstAtom() ];
				const uint indexSecondAtom = p_mapAtomIds[ bond.getIndexSecondAtom() ];

				Model::Bond & generatedBond = addBond();

				generatedBond.setIndexFirstAtom( indexFirstAtom );
				generatedBond.setIndexSecondAtom( indexSecondAtom );
				generatedBond.setOrder( bond.getOrder() );

				getBufferBonds().emplace_back( indexFirstAtom );
				getBufferBonds().emplace_back( indexSecondAtom );

				const uint generatedBondIndex = getBondCount() - 1;

				Model::Residue * const currentResidue	   = getAtom( indexFirstAtom )->getResiduePtr();
				const uint			   currentResidueIndex = currentResidue->getIndex();

				if ( currentResidueIndex > previousResidueIndex )
				{
					previousResidue->setBondCount( counter );
					currentResidue->setIndexFirstBond( generatedBondIndex );

					previousResidue		 = currentResidue;
					previousResidueIndex = currentResidueIndex;

					counter = 1;
				}
				else
				{
					counter++;
				}
			}
		}

		previousResidue->setBondCount( counter );
	}

	void GeneratedMolecule::_validateBuffers()
	{
		getBufferAtomVisibilities().resize( getAtomCount(), 1u );
		getBufferAtomSelections().resize( getAtomCount(), 0u );

		getBufferAtomRadius().shrink_to_fit();
		getBufferAtomIds().shrink_to_fit();
		for ( AtomPositionsFrame & atomPositions : getAtomPositionFrames() )
			atomPositions.shrink_to_fit();

		getBufferBonds().shrink_to_fit();
	}

	void GeneratedMolecule::extractFromSelection( const Model::Selection & p_selection, const Model::ID & p_moleculeID )
	{
		Tool::Chrono chrono = Tool::Chrono();
		chrono.start();

		Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( p_moleculeID );
		const Model::Selection::MapChainIds & moleculeSelectionData = p_selection.getMoleculesMap().at( p_moleculeID );

		_pendingExternalBonds.clear();
		_pendingExternalBonds.reserve( molecule.getResidueCount() );
		//_extractingBonds.reserve( molecule.getBondCount() );
		getBufferBonds().reserve( molecule.getBondCount() * 2u );

		_copyMoleculeData( molecule, "Extract of " );

		for ( const std::pair<const ID, const Model::Selection::MapResidueIds> & chainData : moleculeSelectionData )
		{
			const Model::Chain * const chain = molecule.getChain( chainData.first );

			if ( chain->getRealResidueCount() == chainData.second.getFullySelectedChildCount() )
			{
				_extractFullChain( molecule, chainData.first );
				continue;
			}

			Model::Chain & generatedChain = addChain();
			_copyChainData( generatedChain, *chain );
			generatedChain.setResidueCount( uint( chainData.second.size() ) );

			for ( const std::pair<const ID, const Model::Selection::VecAtomIds> & residueData : chainData.second )
			{
				const Model::Residue * const residue = molecule.getResidue( residueData.first );
				if ( residue->getRealAtomCount() == residueData.second.getFullySelectedChildCount() )
				{
					_extractFullResidue( molecule, residueData.first, &generatedChain );
					continue;
				}

				Model::Residue & generatedResidue = addResidue();
				_copyResidueData( generatedResidue, *residue, &generatedChain );
				_extractAtomsFromResidue( molecule, &generatedResidue, residueData.second, false );
				generatedResidue.setAtomCount( uint( residueData.second.getFullySelectedChildCount() ) );
			}
		}

		_clearPendingExtractedBonds();
		_validateBuffers();

		chrono.stop();

		VTX_INFO( "Extract done in : " + std::to_string( chrono.elapsedTime() ) + "s" );

		molecule.notifyStructureChange();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();
	};

	void GeneratedMolecule::_clearPendingExtractedBonds()
	{
		for ( const ExternalBondExtractData & bondData : _pendingExternalBonds )
		{
			removeBond( bondData.getIndexInExtractedResidue(), true, false );
		}

		_pendingExternalBonds.clear();
	}

	void GeneratedMolecule::extractChain( const Model::Chain & p_chain )
	{
		Tool::Chrono chrono = Tool::Chrono();
		chrono.start();

		Model::Molecule & molecule = *p_chain.getMoleculePtr();

		const uint firstBondIndex = molecule.getResidue( p_chain.getIndexFirstResidue() )->getIndexFirstBond();
		const uint lastBondIndex  = molecule.getResidue( p_chain.getIndexLastResidue() )->getIndexFirstBond()
								   + molecule.getResidue( p_chain.getIndexLastResidue() )->getBondCount();
		const uint maxBondsCount = lastBondIndex - firstBondIndex;

		_pendingExternalBonds.clear();
		_pendingExternalBonds.reserve( p_chain.getResidueCount() );
		getBufferBonds().reserve( maxBondsCount * 2u );
		_copyMoleculeData( molecule, "Extract of " );

		_extractFullChain( molecule, p_chain.getIndex() );

		_clearPendingExtractedBonds();
		_validateBuffers();

		chrono.stop();

		VTX_INFO( "Extract done in : " + std::to_string( chrono.elapsedTime() ) + "s" );

		molecule.forceNotifyDataChanged();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();
	}
	void GeneratedMolecule::extractResidue( const Model::Residue & p_residue )
	{
		Tool::Chrono chrono = Tool::Chrono();
		chrono.start();

		_pendingExternalBonds.clear();
		_pendingExternalBonds.reserve( 4 ); // Considering max 4 external bounds for a residue
		getBufferBonds().reserve( p_residue.getBondCount() * 2u );

		Model::Molecule & molecule = *p_residue.getMoleculePtr();
		_copyMoleculeData( molecule, "Extract of " );

		Model::Chain & generatedChain = addChain();
		_copyChainData( generatedChain, *p_residue.getChainPtr() );
		generatedChain.setResidueCount( 1 );

		_extractFullResidue( molecule, p_residue.getIndex(), &generatedChain );

		_clearPendingExtractedBonds();
		_validateBuffers();

		chrono.stop();

		VTX_INFO( "Extract done in : " + std::to_string( chrono.elapsedTime() ) + "s" );

		molecule.forceNotifyDataChanged();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();
	}
	void GeneratedMolecule::extractAtom( const Model::Atom & p_atom )
	{
		Tool::Chrono chrono;
		chrono.start();

		Model::Molecule & molecule = *p_atom.getMoleculePtr();
		_copyMoleculeData( molecule, "Extract of " );

		const Model::Chain & chain			= *p_atom.getChainPtr();
		Model::Chain &		 generatedChain = addChain();
		_copyChainData( generatedChain, chain );
		generatedChain.setResidueCount( 1 );

		const Model::Residue & residue			= *p_atom.getResiduePtr();
		Model::Residue &	   generatedResidue = addResidue();
		_copyResidueData( generatedResidue, residue, &generatedChain );
		generatedResidue.setAtomCount( 1 );

		_extractAtomsFromResidue( molecule, &generatedResidue, p_atom.getIndex(), 1, false );

		_clearPendingExtractedBonds();
		_validateBuffers();

		molecule.forceNotifyDataChanged();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();

		chrono.stop();

		VTX_INFO( "Extract done in : " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void GeneratedMolecule::_copyMoleculeData( const Model::Molecule & p_molecule, const std::string & p_namePrefix )
	{
		// Copy molecule properties.
		setName( p_namePrefix + p_molecule.getName() );
		setPdbIdCode( "none" );
		setDisplayName( p_namePrefix + p_molecule.getDefaultName() );
		setColor( Color::Rgb::randomPastel() );

		VTX::Representation::RepresentationManager::get().instantiateCopy(
			p_molecule.getRepresentation(), *this, false, false );

		for ( int i = 0; i < p_molecule.getAtomPositionFrames().size(); i++ )
		{
			const AtomPositionsFrame & atomPosFrame			 = p_molecule.getAtomPositionFrames()[ i ];
			AtomPositionsFrame &	   generatedAtomPosFrame = addAtomPositionFrame();
			generatedAtomPosFrame.reserve( atomPosFrame.size() );
		}

		for ( const UnknownResidueData & unknownSymbol : p_molecule.getUnknownResidueSymbols() )
			addUnknownResidueSymbol( unknownSymbol );
		for ( const std::string & unknownSymbol : p_molecule.getUnknownAtomSymbols() )
			addUnknownAtomSymbol( unknownSymbol );

		getBufferAtomRadius().reserve( p_molecule.getAtomCount() );
		getBufferAtomIds().reserve( p_molecule.getAtomCount() );
		_transform = p_molecule.getTransform();

		setFPS( p_molecule.getFPS() );
		setPlayMode( p_molecule.getPlayMode() );
		setFrame( p_molecule.getFrame() );
		setIsPlaying( p_molecule.isPlaying() );
	}
	void GeneratedMolecule::_copyChainData( Model::Chain & p_chain, const Model::Chain & p_chainSource )
	{
		p_chain.setMoleculePtr( this );
		p_chain.setIndex( getChainCount() - 1 );
		p_chain.setName( p_chainSource.getName() );
		p_chain.setOriginalChainID( p_chainSource.getOriginalChainID() );
		p_chain.setIndexFirstResidue( getResidueCount() );
		p_chain.setColor( Model::Chain::getChainIdColor( p_chainSource.getOriginalChainID() ) );

		if ( p_chainSource.hasCustomRepresentation() )
		{
			VTX::Representation::RepresentationManager::get().instantiateCopy(
				p_chainSource.getRepresentation(), *this, false, false );
		}
	}
	void GeneratedMolecule::_copyResidueData( Model::Residue &		 p_residue,
											  const Model::Residue & p_residueSource,
											  Model::Chain * const	 p_parent )
	{
		p_residue.setIndex( getResidueCount() - 1 );
		p_residue.setChainPtr( p_parent );
		p_residue.setIndexInOriginalChain( p_residueSource.getIndexInOriginalChain() );
		p_residue.setIndexFirstAtom( getAtomCount() );
		p_residue.setSymbol( p_residueSource.getSymbol() );
		p_residue.setColor( Model::Residue::getResidueColor( p_residueSource ) );
		p_residue.setType( p_residueSource.getType() );
		p_residue.setSecondaryStructure( p_residueSource.getSecondaryStructure() );
		p_residue.setAtomType( p_residueSource.getAtomType() );

		if ( p_residueSource.hasCustomRepresentation() )
		{
			VTX::Representation::RepresentationManager::get().instantiateCopy(
				p_residueSource.getRepresentation(), *this, false, false );
		}
	}
	void GeneratedMolecule::_copyAtomData( Model::Atom &		  p_atom,
										   const Model::Atom &	  p_atomSource,
										   Model::Residue * const p_parent )
	{
		p_atom.setIndex( getAtomCount() - 1 );
		p_atom.setResiduePtr( p_parent );
		p_atom.setSymbol( p_atomSource.getSymbol() );
		p_atom.setName( p_atomSource.getName() );
		// p_atom.setColor( Model::Atom::SYMBOL_COLOR[ int( p_atomSource.getSymbol() ) ] );
		p_atom.setType( p_atomSource.getType() );
		getBufferAtomRadius().emplace_back( p_atomSource.getVdwRadius() );
		getBufferAtomIds().emplace_back( p_atom.getId() );
	}

	Model::Chain & GeneratedMolecule::_extractFullChain( Model::Molecule & p_fromMolecule, const uint p_index )
	{
		Model::Chain & chain					 = *p_fromMolecule.getChain( p_index );
		const uint	   previousFirstResidueIndex = chain.getIndexFirstResidue();
		const uint	   indexFirstResidue		 = getResidueCount();

		_addChain( &chain );

		// Check contiguous atom ranges
		uint firstResidueIndex = previousFirstResidueIndex;

		uint	   firstAtomIndexRange = p_fromMolecule.getResidue( previousFirstResidueIndex )->getIndexFirstAtom();
		uint	   atomRangeCount	   = p_fromMolecule.getResidue( previousFirstResidueIndex )->getAtomCount();
		const uint lastResidueIndex	   = previousFirstResidueIndex + chain.getResidueCount() - 1;
		for ( uint i = previousFirstResidueIndex + 1; i <= lastResidueIndex; i++ )
		{
			const Model::Residue & currResidue	  = *p_fromMolecule.getResidue( i );
			const uint			   indexFirstAtom = currResidue.getIndexFirstAtom();

			if ( indexFirstAtom == ( firstAtomIndexRange + atomRangeCount ) )
			{
				atomRangeCount += currResidue.getAtomCount();
			}
			else
			{
				_extractAllResidues( p_fromMolecule, firstResidueIndex, i - firstResidueIndex );

				firstAtomIndexRange = indexFirstAtom;
				atomRangeCount		= currResidue.getAtomCount();
				firstResidueIndex	= i;
			}
		}

		if ( atomRangeCount > 0 )
			_extractAllResidues( p_fromMolecule, firstResidueIndex, lastResidueIndex - firstResidueIndex + 1 );

		chain.setMoleculePtr( this );
		chain.setIndex( getChainCount() - 1 );
		chain.setIndexFirstResidue( indexFirstResidue );

		p_fromMolecule.removeChain( p_index, false, false, false );

		return chain;
	}

	void GeneratedMolecule::_extractAllResidues( Model::Molecule & p_fromMolecule,
												 const uint		   p_startIndex,
												 const uint		   p_count )
	{
		const Model::Residue * const firstSourceResidue	  = p_fromMolecule.getResidue( p_startIndex );
		const Model::Residue * const lastSourceResidue	  = p_fromMolecule.getResidue( p_startIndex + p_count - 1 );
		const uint					 indexFirstSourceAtom = firstSourceResidue->getIndexFirstAtom();
		const uint indexLastSourceAtom = lastSourceResidue->getIndexFirstAtom() + lastSourceResidue->getAtomCount() - 1;

		const uint indexFirstResidue = getResidueCount();

		// Insert residues
		getResidues().insert( getResidues().end(),
							  p_fromMolecule.getResidues().begin() + p_startIndex,
							  p_fromMolecule.getResidues().begin() + p_startIndex + p_count );

		// Extract Atoms
		_extractAllAtoms( p_fromMolecule, indexFirstSourceAtom, indexLastSourceAtom - indexFirstSourceAtom + 1 );

		// Remove Residues from extracted molecule and apply right params on added residue
		for ( uint i = 0; i < p_count; i++ )
		{
			const uint currentResidueIndex	= indexFirstResidue + i;
			const uint previousResidueIndex = p_startIndex + i;

			p_fromMolecule.removeResidue( previousResidueIndex, false, false, false, false );

			Model::Residue & residue = *getResidue( currentResidueIndex );
			residue.setIndex( currentResidueIndex );
			residue.setRepresentableMolecule( this );
		}
	}

	Model::Residue & GeneratedMolecule::_extractFullResidue( Model::Molecule &	  p_fromMolecule,
															 const uint			  p_index,
															 Model::Chain * const p_parent )
	{
		Model::Residue & residue				= *p_fromMolecule.getResidue( p_index );
		const uint		 previousIndex			= residue.getIndex();
		const uint		 previousIndexFirstAtom = residue.getIndexFirstAtom();
		const uint		 indexFirstAtom			= getAtomCount();

		getResidues().emplace_back( &residue );
		_extractAtomsFromResidue( p_fromMolecule, &residue, previousIndexFirstAtom, residue.getAtomCount(), true );

		p_fromMolecule.removeResidue( previousIndex, false, false, true, false );
		residue.setIndex( getResidueCount() - 1 );
		residue.setChainPtr( p_parent );
		residue.setIndexFirstAtom( indexFirstAtom );

		return residue;
	}

	void GeneratedMolecule::_extractAllAtoms( Model::Molecule & p_fromMolecule,
											  const uint		p_startIndex,
											  const uint		p_count )
	{
		const Model::Residue * const previousFirstResidue = p_fromMolecule.getAtom( p_startIndex )->getResiduePtr();
		const Model::Residue * const previousLastResidue
			= p_fromMolecule.getAtom( p_startIndex + p_count - 1 )->getResiduePtr();

		const uint previousIndexFirstBond = previousFirstResidue->getIndexFirstBond();
		const uint previousIndexLastBond
			= previousLastResidue->getIndexFirstBond() + previousLastResidue->getBondCount() - 1;
		const uint bondCount = previousIndexLastBond - previousIndexFirstBond + 1;

		const uint indexFirstAtom = getAtomCount();
		const uint indexFirstBond = getBondCount();

		const uint atomOffset = previousFirstResidue->getIndexFirstAtom() - indexFirstAtom;
		const uint bondOffset = previousFirstResidue->getIndexFirstBond() - indexFirstBond;

		getAtoms().insert( getAtoms().end(),
						   p_fromMolecule.getAtoms().begin() + p_startIndex,
						   p_fromMolecule.getAtoms().begin() + p_startIndex + p_count );

		getBufferAtomRadius().insert( getBufferAtomRadius().end(),
									  p_fromMolecule.getBufferAtomRadius().begin() + p_startIndex,
									  p_fromMolecule.getBufferAtomRadius().begin() + p_startIndex + p_count );

		for ( uint i = 0; i < getAtomPositionFrames().size(); i++ )
		{
			getAtomPositionFrame( i ).insert(
				getAtomPositionFrame( i ).end(),
				p_fromMolecule.getAtomPositionFrame( i ).begin() + p_startIndex,
				p_fromMolecule.getAtomPositionFrame( i ).begin() + p_startIndex + p_count );
		}

		getBonds().insert( getBonds().end(),
						   p_fromMolecule.getBonds().begin() + previousIndexFirstBond,
						   p_fromMolecule.getBonds().begin() + previousIndexLastBond + 1 );

		getBufferBonds().insert( getBufferBonds().end(),
								 p_fromMolecule.getBufferBonds().begin() + previousIndexFirstBond * 2,
								 p_fromMolecule.getBufferBonds().begin() + ( previousIndexLastBond * 2 + 1 ) + 1 );

		for ( uint i = indexFirstBond; i < indexFirstBond + bondCount; i++ )
		{
			if ( getBond( i ) == nullptr )
				continue;

			getBond( i )->setIndexFirstAtom( getBond( i )->getIndexFirstAtom() - atomOffset );
			getBond( i )->setIndexSecondAtom( getBond( i )->getIndexSecondAtom() - atomOffset );
			getBufferBonds()[ i * 2u ] -= atomOffset;
			getBufferBonds()[ i * 2u + 1u ] -= atomOffset;

			p_fromMolecule.getBonds()[ bondOffset + i ] = nullptr;
		}

		for ( uint i = previousFirstResidue->getIndex(); i <= previousLastResidue->getIndex(); i++ )
		{
			Model::Residue * const residue = p_fromMolecule.getResidue( i );
			residue->setIndexFirstAtom( residue->getIndexFirstAtom() - atomOffset );
			residue->setIndexFirstBond( residue->getIndexFirstBond() - bondOffset );
		}

		for ( uint i = indexFirstAtom; i < getAtomCount(); i++ )
		{
			Model::Atom & atom = *getAtom( i );
			atom.setIndex( i );
			getBufferAtomIds().emplace_back( atom.getId() );
		}

		for ( uint i = p_startIndex; i < p_startIndex + p_count; i++ )
			p_fromMolecule.removeAtom( i, false, false, false, false );
	}

	void GeneratedMolecule::_extractAtomsFromResidue( Model::Molecule &		 p_fromMolecule,
													  Model::Residue * const p_parent,
													  const uint			 p_startIndex,
													  const uint			 p_count,
													  bool					 p_parentFromMolecule )
	{
		std::vector<uint> vec = std::vector<uint>();
		vec.resize( p_count );
		for ( uint i = 0; i < p_count; i++ )
			vec[ i ] = p_startIndex + i;
		_extractAtomsFromResidue( p_fromMolecule, p_parent, vec, p_parentFromMolecule );
	}

	void GeneratedMolecule::_extractAtomsFromResidue( Model::Molecule &			p_fromMolecule,
													  Model::Residue * const	p_parent,
													  const std::vector<uint> & p_indexes,
													  bool						p_parentFromMolecule )
	{
		const uint indexFirstAtom	 = getAtomCount();
		const uint newIndexFirstBond = getBondCount();

		const Model::Residue & sourceResidue	  = *p_fromMolecule.getAtom( p_indexes[ 0 ] )->getResiduePtr();
		const uint			   parentFirstAtom	  = sourceResidue.getIndexFirstAtom();
		const uint			   parentFirstAtomOut = sourceResidue.getIndexFirstAtom() + sourceResidue.getAtomCount();

		std::vector<BondExtractData> internalBonds = std::vector<BondExtractData>();
		internalBonds.reserve( sourceResidue.getBondCount() );

		std::vector<ExternalBondExtractData> externalBonds = std::vector<ExternalBondExtractData>();
		externalBonds.reserve( 4 );

		for ( const uint idAtom : p_indexes )
		{
			Model::Atom &		   atom			= *p_fromMolecule.getAtom( idAtom );
			const Model::Residue & fromResidue	= *atom.getResiduePtr();
			const uint			   newAtomIndex = getAtomCount();

			for ( uint i = 0; i < internalBonds.size(); i++ )
			{
				BondExtractData & bondData = internalBonds[ i ];

				if ( !bondData.isFirstIndexLinked() && bondData.getBond()->getIndexFirstAtom() == idAtom )
				{
					bondData.setFirstIndex( newAtomIndex );
					p_fromMolecule.removeBond( bondData.getPreviousBondIndex(), false, false );
					_extractBond( bondData );
				}
				else if ( !bondData.isSecondIndexLinked() && bondData.getBond()->getIndexSecondAtom() == idAtom )
				{
					bondData.setSecondIndex( newAtomIndex );
					p_fromMolecule.removeBond( bondData.getPreviousBondIndex(), false, false );
					_extractBond( bondData );
				}
			}

			for ( uint idBond = fromResidue.getIndexFirstBond();
				  idBond < fromResidue.getIndexFirstBond() + fromResidue.getBondCount();
				  idBond++ )
			{
				Model::Bond * const bond = p_fromMolecule.getBond( idBond );

				if ( bond == nullptr )
					continue;

				if ( bond->getIndexFirstAtom() == idAtom )
				{
					const uint otherAtomIndexInParent = bond->getIndexSecondAtom();

					const bool isExternalBond
						= otherAtomIndexInParent < parentFirstAtom || otherAtomIndexInParent >= parentFirstAtomOut;

					if ( isExternalBond )
					{
						ExternalBondExtractData bondData = ExternalBondExtractData( bond, idBond );
						bondData.setFirstIndex( newAtomIndex );

						externalBonds.emplace_back( bondData );
					}
					else
					{
						BondExtractData bondData = BondExtractData( bond, idBond );
						bondData.setFirstIndex( newAtomIndex );
						internalBonds.emplace_back( bondData );
					}
				}
				else if ( bond->getIndexSecondAtom() == idAtom )
				{
					const uint otherAtomIndexInParent = bond->getIndexFirstAtom();
					const bool isExternalBond
						= otherAtomIndexInParent < parentFirstAtom || otherAtomIndexInParent >= parentFirstAtomOut;

					if ( isExternalBond )
					{
						ExternalBondExtractData bondData = ExternalBondExtractData( bond, idBond );
						bondData.setSecondIndex( newAtomIndex );

						externalBonds.emplace_back( bondData );
					}
					else
					{
						BondExtractData bondData = BondExtractData( bond, idBond );
						bondData.setSecondIndex( newAtomIndex );
						internalBonds.emplace_back( bondData );
					}
				}
			}

			// Only emplace atom currently to not modify it before checking all bonds
			getAtoms().emplace_back( &atom );
			getBufferAtomRadius().emplace_back( atom.getVdwRadius() );
			getBufferAtomIds().emplace_back( atom.getId() );

			for ( uint i = 0; i < getAtomPositionFrames().size(); i++ )
				getAtomPositionFrame( i ).emplace_back( p_fromMolecule.getAtomPositionFrame( i )[ idAtom ] );
		}

		for ( ExternalBondExtractData & bondData : externalBonds )
		{
			std::vector<ExternalBondExtractData>::iterator itExternalBond
				= _findInPendingExternalBond( *bondData.getBond() );

			if ( itExternalBond != _pendingExternalBonds.end() )
			{
				ExternalBondExtractData & pendingExternalBondData = *itExternalBond;

				if ( pendingExternalBondData.isFirstIndexLinked() && bondData.isSecondIndexLinked() )
				{
					bondData.setFirstIndex( pendingExternalBondData.getFirstIndex() );
					pendingExternalBondData.setSecondIndex( bondData.getSecondIndex() );
				}
				else if ( pendingExternalBondData.isSecondIndexLinked() && bondData.isFirstIndexLinked() )
				{
					pendingExternalBondData.setFirstIndex( bondData.getFirstIndex() );
					bondData.setSecondIndex( pendingExternalBondData.getSecondIndex() );
				}

				_applyExternalBond( pendingExternalBondData );

				p_fromMolecule.removeBond( bondData.getPreviousBondIndex(), false, false );
				_extractBond( bondData );

				_pendingExternalBonds.erase( itExternalBond );
			}
			else
			{
				// Emplace bonds. Will be removed if external residue not link in extract
				bondData.setIndexInExtractedResidue( getBondCount() );
				getBonds().emplace_back( bondData.getBond() );
				getBufferBonds().emplace_back( -1 );
				getBufferBonds().emplace_back( -1 );

				p_fromMolecule.removeBond( bondData.getPreviousBondIndex(), false, false );

				_pendingExternalBonds.emplace_back( bondData );
			}
		}

		// Clean bonds between previous molecule and new one
		for ( BondExtractData & bondData : internalBonds )
		{
			if ( !bondData.hasToBeExtracted() )
				p_fromMolecule.removeBond( bondData.getPreviousBondIndex(), true, false );
		}

		// Remove Atoms
		for ( const uint idAtom : p_indexes )
		{
			p_fromMolecule.removeAtom( idAtom, false, !p_parentFromMolecule, !p_parentFromMolecule, false );
		}

		// Update atom data
		for ( uint i = indexFirstAtom; i < getAtomCount(); i++ )
		{
			Model::Atom & atom = *getAtom( i );

			atom.setIndex( i );
			atom.setResiduePtr( p_parent );
		}

		// Update residue parent data
		p_parent->setIndexFirstBond( newIndexFirstBond );
		p_parent->setBondCount( getBondCount() - p_parent->getIndexFirstBond() );
	}

	void GeneratedMolecule::_extractBond( const BondExtractData & p_bondData )
	{
		Model::Bond * const bond = p_bondData.getBond();

		const uint newFirstIndex  = p_bondData.getFirstIndex();
		const uint newSecondIndex = p_bondData.getSecondIndex();
		bond->setIndexFirstAtom( newFirstIndex );
		bond->setIndexSecondAtom( newSecondIndex );
		bond->setMoleculePtr( this );

		getBonds().emplace_back( bond );
		getBufferBonds().emplace_back( newFirstIndex );
		getBufferBonds().emplace_back( newSecondIndex );
	}

	void GeneratedMolecule::_applyExternalBond( const ExternalBondExtractData & p_externalBondData )
	{
		p_externalBondData.getBond()->setIndexFirstAtom( p_externalBondData.getFirstIndex() );
		p_externalBondData.getBond()->setIndexSecondAtom( p_externalBondData.getSecondIndex() );
		p_externalBondData.getBond()->setMoleculePtr( this );

		getBufferBonds()[ p_externalBondData.getIndexInExtractedResidue() * 2 ] = p_externalBondData.getFirstIndex();
		getBufferBonds()[ p_externalBondData.getIndexInExtractedResidue() * 2 + 1 ]
			= p_externalBondData.getSecondIndex();
	}

	std::vector<GeneratedMolecule::ExternalBondExtractData>::iterator GeneratedMolecule::_findInPendingExternalBond(
		const Model::Bond & p_bond )
	{
		std::vector<ExternalBondExtractData>::iterator it = _pendingExternalBonds.begin();

		while ( it != _pendingExternalBonds.end() )
		{
			ExternalBondExtractData & externalBondData = *it;

			if ( externalBondData.getBond()->getIndexFirstAtom() == p_bond.getIndexFirstAtom()
				 && externalBondData.getBond()->getIndexFirstAtom() == p_bond.getIndexFirstAtom() )
			{
				return it;
			}
			it++;
		}

		return _pendingExternalBonds.end();
	}
} // namespace VTX::Model
