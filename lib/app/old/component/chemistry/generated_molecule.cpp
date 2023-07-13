#include "app/old/component/chemistry/generated_molecule.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/component/chemistry/atom.hpp"
#include "app/old/component/chemistry/bond.hpp"
#include "app/old/component/chemistry/category.hpp"
#include "app/old/component/chemistry/chain.hpp"
#include "app/old/component/chemistry/residue.hpp"
#include "app/old/id.hpp"
#include "app/old/mvc.hpp"
#include <map>
#include <util/chrono.hpp>
#include <util/constants.hpp>

namespace VTX::App::Old::Component::Chemistry
{
	GeneratedMolecule::BondExtractData::BondExtractData( Chemistry::Bond * const p_bond,
														 const uint				 p_previousBondIndex ) :
		_bond( p_bond ),
		_previousBondIndex( p_previousBondIndex ), _sourceMolecule( p_bond->getMoleculePtr() ),
		_sourceFirstIndex( p_bond->getIndexFirstAtom() ), _sourceSecondIndex( p_bond->getIndexSecondAtom() )
	{
	}

	GeneratedMolecule::GeneratedMolecule() : Chemistry::Molecule( App::Old::ID::Model::MODEL_GENERATED_MOLECULE ) {}

	void GeneratedMolecule::copyFromSelection( const App::Old::Application::Selection::SelectionModel & p_selection,
											   const VTX::App::Old::Core::Model::ID &					p_moleculeID,
											   const int												p_frame )
	{
		Util::Chrono chrono;
		chrono.start();

		const Chemistry::Molecule & molecule = VTX::MVC_MANAGER().getModel<Chemistry::Molecule>( p_moleculeID );
		const App::Old::Application::Selection::SelectionModel::MapChainIds & moleculeSelectionData
			= p_selection.getMoleculesMap().at( p_moleculeID );

		const std::string prefix = p_frame == ALL_FRAMES_INDEX ? COPY_PREFIX : _getFrameCopyPrefix( p_frame );
		const std::string suffix = p_frame == ALL_FRAMES_INDEX ? COPY_SUFFIX : _getFrameCopySuffix( p_frame );

		_copyMoleculeData( molecule, p_frame, prefix, suffix );

		_mapAtomIds = std::map<const uint, const uint>();

		for ( const std::pair<const VTX::App::Old::Core::Model::ID,
							  App::Old::Application::Selection::SelectionModel::MapResidueIds> & chainData :
			  moleculeSelectionData )
		{
			const Chemistry::Chain & chain			= *molecule.getChain( chainData.first );
			Chemistry::Chain &		 generatedChain = addChain();

			_copyChainData( generatedChain, chain );

			generatedChain.setResidueCount( uint( chainData.second.size() ) );

			for ( const std::pair<const VTX::App::Old::Core::Model::ID,
								  App::Old::Application::Selection::SelectionModel::VecAtomIds> & residueData :
				  chainData.second )
			{
				const Chemistry::Residue & residue			= *molecule.getResidue( residueData.first );
				Chemistry::Residue &	   generatedResidue = addResidue();
				_copyResidueData( generatedResidue, residue, &generatedChain );
				generatedResidue.setAtomCount( uint( residueData.second.size() ) );

				for ( const App::Old::Core::Model::ID & atomId : residueData.second )
				{
					const Chemistry::Atom & atom = *molecule.getAtom( atomId );

					Chemistry::Atom & generatedAtom = addAtom();
					generatedAtom.setResiduePtr( &generatedResidue );

					_copyAtomData( generatedAtom, atom, &generatedResidue );

					_mapAtomIds.emplace( atom.getIndex(), generatedAtom.getIndex() );

					if ( p_frame == ALL_FRAMES_INDEX )
					{
						// Copy atom position for each frame
						for ( int i = 0; i < getAtomPositionFrames().size(); i++ )
						{
							getAtomPositionFrame( i ).emplace_back(
								molecule.getAtomPositionFrame( i )[ atom.getIndex() ] );
						}
					}
					else
					{
						getAtomPositionFrame( 0 ).emplace_back(
							molecule.getAtomPositionFrame( p_frame )[ atom.getIndex() ] );
					}
				}
			}
		}

		_computeBonds( molecule );

		getBufferAtomColors().resize( getAtomCount(), COLOR_BLUE );
		getBufferAtomVisibilities().resize( getAtomCount(), 1u );
		getBufferAtomSelections().resize( getAtomCount(), 0u );

		getBufferAtomRadius().shrink_to_fit();
		getBufferAtomIds().shrink_to_fit();
		for ( AtomPositionsFrame & atomPositions : getAtomPositionFrames() )
			atomPositions.shrink_to_fit();

		chrono.stop();
		VTX_DEBUG( "Copy done in {}.", chrono.elapsedTimeStr() );
	}

	void GeneratedMolecule::copyFromMolecule( const Chemistry::Molecule & p_molecule, const int p_frame )
	{
		Util::Chrono chrono;
		chrono.start();

		const std::string prefix = p_frame == ALL_FRAMES_INDEX ? COPY_PREFIX : _getFrameCopyPrefix( p_frame );
		const std::string suffix = p_frame == ALL_FRAMES_INDEX ? COPY_SUFFIX : _getFrameCopySuffix( p_frame );

		_copyFullMolecule( p_molecule, p_frame, prefix, suffix );

		_computeBonds( p_molecule );
		_validateBuffers();

		chrono.stop();
		VTX_DEBUG( "Copy done in {}.", chrono.elapsedTimeStr() );
	}
	void GeneratedMolecule::copyFromChain( const Chemistry::Chain & p_chain, const int p_frame )
	{
		Util::Chrono chrono;
		chrono.start();

		const Chemistry::Molecule & molecule = *p_chain.getMoleculePtr();

		const std::string prefix = p_frame == ALL_FRAMES_INDEX ? COPY_PREFIX : _getFrameCopyPrefix( p_frame );
		const std::string suffix = p_frame == ALL_FRAMES_INDEX ? COPY_SUFFIX : _getFrameCopySuffix( p_frame );

		_copyMoleculeData( molecule, p_frame, prefix, suffix );

		_mapAtomIds = std::map<const uint, const uint>();
		_copyFullChain( p_chain, molecule );

		_computeBonds( molecule );
		_validateBuffers();

		chrono.stop();
		VTX_DEBUG( "Copy done in {}.", chrono.elapsedTimeStr() );
	}
	void GeneratedMolecule::copyFromResidue( const Chemistry::Residue & p_residue, const int p_frame )
	{
		Util::Chrono chrono;
		chrono.start();

		const Chemistry::Molecule & molecule = *p_residue.getMoleculePtr();

		const std::string prefix = p_frame == ALL_FRAMES_INDEX ? COPY_PREFIX : _getFrameCopyPrefix( p_frame );
		const std::string suffix = p_frame == ALL_FRAMES_INDEX ? COPY_SUFFIX : _getFrameCopySuffix( p_frame );

		_copyMoleculeData( molecule, p_frame, prefix, suffix );

		const Chemistry::Chain & chain			= *p_residue.getChainPtr();
		Chemistry::Chain &		 generatedChain = addChain();
		_copyChainData( generatedChain, chain );
		generatedChain.setResidueCount( 1 );

		_mapAtomIds = std::map<const uint, const uint>();
		_copyFullResidue( p_residue, molecule, &generatedChain );

		_computeBonds( molecule );
		_validateBuffers();

		_mapAtomIds.clear();

		chrono.stop();
		VTX_DEBUG( "Copy done in {}.", chrono.elapsedTimeStr() );
	}
	void GeneratedMolecule::copyFromAtom( const Chemistry::Atom & p_atom, const int p_frame )
	{
		Util::Chrono chrono;
		chrono.start();

		const Chemistry::Molecule & molecule = *p_atom.getMoleculePtr();

		const std::string prefix = p_frame == ALL_FRAMES_INDEX ? COPY_PREFIX : _getFrameCopyPrefix( p_frame );
		const std::string suffix = p_frame == ALL_FRAMES_INDEX ? COPY_SUFFIX : _getFrameCopySuffix( p_frame );

		_copyMoleculeData( molecule, p_frame, prefix, suffix );

		const Chemistry::Chain & chain			= *p_atom.getChainPtr();
		Chemistry::Chain &		 generatedChain = addChain();
		_copyChainData( generatedChain, chain );
		generatedChain.setResidueCount( 1 );

		const Chemistry::Residue & residue			= *p_atom.getResiduePtr();
		Chemistry::Residue &	   generatedResidue = addResidue();
		_copyResidueData( generatedResidue, residue, &generatedChain );
		generatedResidue.setAtomCount( 1 );

		_copyAtom( p_atom, molecule, &generatedResidue );

		_validateBuffers();

		chrono.stop();
		VTX_DEBUG( "Copy done in {}.", chrono.elapsedTimeStr() );
	}

	void GeneratedMolecule::_copyFullMolecule( const Chemistry::Molecule & p_moleculeSource,
											   const int				   p_frame,
											   const std::string &		   p_prefix,
											   const std::string &		   p_suffix )
	{
		_copyMoleculeData( p_moleculeSource, p_frame, p_prefix, p_suffix );

		for ( uint i = 0; i < p_moleculeSource.getChainCount(); i++ )
		{
			const Chemistry::Chain * const chain = p_moleculeSource.getChain( i );

			if ( chain == nullptr )
				continue;

			_copyFullChain( *chain, p_moleculeSource, p_frame );
		}
	}
	Chemistry::Chain & GeneratedMolecule::_copyFullChain( const Chemistry::Chain &	  p_source,
														  const Chemistry::Molecule & p_moleculeSource,
														  const int					  p_frame )
	{
		Chemistry::Chain & generatedChain = addChain();
		_copyChainData( generatedChain, p_source );

		const uint firstIndex = p_source.getIndexFirstResidue();
		const uint lastIndex  = p_source.getIndexLastResidue();

		uint residueCount = 0;

		for ( uint i = firstIndex; i <= lastIndex; i++ )
		{
			const Chemistry::Residue * const residue = p_moleculeSource.getResidue( i );

			if ( residue == nullptr )
				continue;

			_copyFullResidue( *residue, p_moleculeSource, &generatedChain, p_frame );
			residueCount++;
		}

		generatedChain.setResidueCount( residueCount );
		return generatedChain;
	}

	Chemistry::Residue & GeneratedMolecule::_copyFullResidue( const Chemistry::Residue &  p_source,
															  const Chemistry::Molecule & p_moleculeSource,
															  Chemistry::Chain * const	  p_parent,
															  const int					  p_frame )
	{
		Chemistry::Residue & generatedResidue = addResidue();
		_copyResidueData( generatedResidue, p_source, p_parent );

		const uint firstIndex = p_source.getIndexFirstAtom();
		const uint lastIndex  = p_source.getIndexFirstAtom() + p_source.getAtomCount() - 1;
		uint	   atomCount  = 0;

		for ( uint i = firstIndex; i <= lastIndex; i++ )
		{
			const Chemistry::Atom * const atom = p_moleculeSource.getAtom( i );

			if ( atom == nullptr )
				continue;

			Chemistry::Atom & generatedAtom = _copyAtom( *atom, p_moleculeSource, &generatedResidue, p_frame );
			_mapAtomIds.emplace( atom->getIndex(), generatedAtom.getIndex() );

			atomCount++;
		}

		generatedResidue.setAtomCount( atomCount );

		return generatedResidue;
	}
	Chemistry::Atom & GeneratedMolecule::_copyAtom( const Chemistry::Atom &		p_source,
													const Chemistry::Molecule & p_moleculeSource,
													Chemistry::Residue * const	p_parent,
													const int					p_frame )
	{
		Chemistry::Atom & generatedAtom = addAtom();
		generatedAtom.setResiduePtr( p_parent );
		_copyAtomData( generatedAtom, p_source, p_parent );

		// Copy atom position for each frame
		if ( p_frame == -1 )
		{
			for ( int i = 0; i < getAtomPositionFrames().size(); i++ )
			{
				getAtomPositionFrame( i ).emplace_back(
					p_moleculeSource.getAtomPositionFrame( i )[ p_source.getIndex() ] );
			}
		}
		else
		{
			getAtomPositionFrame( p_frame ).emplace_back(
				p_moleculeSource.getAtomPositionFrame( p_frame )[ p_source.getIndex() ] );
		}

		return generatedAtom;
	}

	void GeneratedMolecule::_computeBonds( const Chemistry::Molecule & p_source )
	{
		if ( p_source.getBondCount() <= 0 )
			return;

		// Bonds
		getBufferBonds().reserve( p_source.getBufferBonds().size() );

		uint					indexFirstValidBond = 0;
		const Chemistry::Bond * bondPtr				= p_source.getBonds()[ indexFirstValidBond ];

		while ( indexFirstValidBond < p_source.getBondCount() && bondPtr == nullptr )
		{
			indexFirstValidBond++;
			bondPtr = p_source.getBonds()[ indexFirstValidBond ];
		}

		if ( bondPtr == nullptr )
			return;

		Chemistry::Residue * previousResidue	  = getAtom( bondPtr->getIndexFirstAtom() )->getResiduePtr();
		uint				 previousResidueIndex = previousResidue->getIndex();
		uint				 counter			  = 0;

		for ( uint i = indexFirstValidBond; i < p_source.getBondCount(); i++ )
		{
			bondPtr = p_source.getBonds()[ i ];

			if ( bondPtr == nullptr )
				continue;

			const Chemistry::Bond & bond = *bondPtr;

			if ( _mapAtomIds.find( bond.getIndexFirstAtom() ) != _mapAtomIds.end()
				 && _mapAtomIds.find( bond.getIndexSecondAtom() ) != _mapAtomIds.end() )
			{
				const uint indexFirstAtom  = _mapAtomIds[ bond.getIndexFirstAtom() ];
				const uint indexSecondAtom = _mapAtomIds[ bond.getIndexSecondAtom() ];

				Chemistry::Bond & generatedBond = addBond();

				generatedBond.setIndexFirstAtom( indexFirstAtom );
				generatedBond.setIndexSecondAtom( indexSecondAtom );
				generatedBond.setOrder( bond.getOrder() );

				getBufferBonds().emplace_back( indexFirstAtom );
				getBufferBonds().emplace_back( indexSecondAtom );

				const uint generatedBondIndex = getBondCount() - 1;

				Chemistry::Residue * const currentResidue	   = getAtom( indexFirstAtom )->getResiduePtr();
				const uint				   currentResidueIndex = currentResidue->getIndex();

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
		getBufferAtomColors().resize( getAtomCount(), COLOR_BLUE );
		getBufferAtomVisibilities().resize( getAtomCount(), 1u );
		getBufferAtomSelections().resize( getAtomCount(), 0u );

		getBufferAtomRadius().shrink_to_fit();
		getBufferAtomIds().shrink_to_fit();
		for ( AtomPositionsFrame & atomPositions : getAtomPositionFrames() )
			atomPositions.shrink_to_fit();

		getBufferBonds().shrink_to_fit();
	}

	void GeneratedMolecule::extractFromSelection( const App::Old::Application::Selection::SelectionModel & p_selection,
												  const VTX::App::Old::Core::Model::ID & p_moleculeID )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();

		Chemistry::Molecule & molecule = VTX::MVC_MANAGER().getModel<Chemistry::Molecule>( p_moleculeID );
		const App::Old::Application::Selection::SelectionModel::MapChainIds & moleculeSelectionData
			= p_selection.getMoleculesMap().at( p_moleculeID );

		_pendingExternalBonds.clear();
		_pendingExternalBonds.reserve( molecule.getResidueCount() );
		//_extractingBonds.reserve( molecule.getBondCount() );
		getBufferBonds().reserve( molecule.getBondCount() * 2u );

		_copyMoleculeData( molecule, ALL_FRAMES_INDEX, EXTRACT_PREFIX, EXTRACT_SUFFIX );

		for ( const std::pair<const VTX::App::Old::Core::Model::ID,
							  App::Old::Application::Selection::SelectionModel::MapResidueIds> & chainData :
			  moleculeSelectionData )
		{
			const Chemistry::Chain * const chain = molecule.getChain( chainData.first );

			if ( chain->getRealResidueCount() == chainData.second.getFullySelectedChildCount() )
			{
				_extractFullChain( molecule, chainData.first );
				continue;
			}

			Chemistry::Chain & generatedChain = addChain();
			_copyChainData( generatedChain, *chain );
			generatedChain.setResidueCount( uint( chainData.second.size() ) );

			for ( const std::pair<const VTX::App::Old::Core::Model::ID,
								  App::Old::Application::Selection::SelectionModel::VecAtomIds> & residueData :
				  chainData.second )
			{
				const Chemistry::Residue * const residue = molecule.getResidue( residueData.first );
				if ( residue->getRealAtomCount() == residueData.second.getFullySelectedChildCount() )
				{
					_extractFullResidue( molecule, residueData.first, &generatedChain );
					continue;
				}

				Chemistry::Residue & generatedResidue = addResidue();
				_copyResidueData( generatedResidue, *residue, &generatedChain );
				_extractAtomsFromResidue( molecule, &generatedResidue, residueData.second, false );
				generatedResidue.setAtomCount( uint( residueData.second.getFullySelectedChildCount() ) );
			}
		}

		_clearPendingExtractedBonds();
		_validateBuffers();

		chrono.stop();

		VTX_DEBUG( "Extract done in {}.", chrono.elapsedTimeStr() );

		molecule.notifyStructureChange();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();
	};

	void GeneratedMolecule::_clearPendingExtractedBonds()
	{
		for ( const ExternalBondExtractData & bondData : _pendingExternalBonds )
		{
			// Remove external bond of other residue
			const uint sourceFirstAtomIndex	 = bondData.getSourceFirstIndex();
			const uint sourceSecondAtomIndex = bondData.getSourceSecondIndex();

			const Chemistry::Atom * const sourceFirstAtom
				= bondData.getSourceMolecule()->getAtom( sourceFirstAtomIndex );
			const Chemistry::Atom * const sourceSecondAtom
				= bondData.getSourceMolecule()->getAtom( sourceSecondAtomIndex );

			if ( sourceFirstAtom != nullptr )
			{
				const uint previousIndex
					= sourceFirstAtom->getResiduePtr()->findBondIndex( sourceFirstAtomIndex, sourceSecondAtomIndex );

				if ( previousIndex != INVALID_ID )
				{
					bondData.getSourceMolecule()->removeBond( previousIndex, true, false );
				}
			}

			if ( sourceSecondAtom != nullptr )
			{
				const uint previousIndex
					= sourceSecondAtom->getResiduePtr()->findBondIndex( sourceFirstAtomIndex, sourceSecondAtomIndex );

				if ( previousIndex != INVALID_ID )
				{
					bondData.getSourceMolecule()->removeBond( previousIndex, true, false );
				}
			}

			// Remove uncomplete residue in extracted residue
			removeBond( bondData.getIndexInExtractedResidue(), true, false );
		}

		_pendingExternalBonds.clear();
		_pendingExternalBonds.shrink_to_fit();
	}

	void GeneratedMolecule::extractChain( const Chemistry::Chain & p_chain )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();

		Chemistry::Molecule & molecule = *p_chain.getMoleculePtr();

		const uint firstBondIndex = molecule.getResidue( p_chain.getIndexFirstResidue() )->getIndexFirstBond();
		const uint lastBondIndex  = molecule.getResidue( p_chain.getIndexLastResidue() )->getIndexFirstBond()
								   + molecule.getResidue( p_chain.getIndexLastResidue() )->getBondCount();
		const uint maxBondsCount = lastBondIndex - firstBondIndex;

		_pendingExternalBonds.clear();
		_pendingExternalBonds.reserve( p_chain.getResidueCount() );
		getBufferBonds().reserve( maxBondsCount * 2u );
		_copyMoleculeData( molecule, ALL_FRAMES_INDEX, EXTRACT_PREFIX, EXTRACT_SUFFIX );

		_extractFullChain( molecule, p_chain.getIndex() );

		_clearPendingExtractedBonds();
		_validateBuffers();

		chrono.stop();

		VTX_DEBUG( "Extract done in {}.", chrono.elapsedTimeStr() );

		molecule.forceNotifyDataChanged();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();
	}
	void GeneratedMolecule::extractResidue( const Chemistry::Residue & p_residue )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();

		_pendingExternalBonds.clear();
		_pendingExternalBonds.reserve( 4 ); // Considering max 4 external bounds for a residue
		getBufferBonds().reserve( p_residue.getBondCount() * 2u );

		Chemistry::Molecule & molecule = *p_residue.getMoleculePtr();
		_copyMoleculeData( molecule, ALL_FRAMES_INDEX, EXTRACT_PREFIX, EXTRACT_SUFFIX );

		Chemistry::Chain & generatedChain = addChain();
		_copyChainData( generatedChain, *p_residue.getChainPtr() );
		generatedChain.setResidueCount( 1 );

		_extractFullResidue( molecule, p_residue.getIndex(), &generatedChain );

		_clearPendingExtractedBonds();
		_validateBuffers();

		chrono.stop();

		VTX_DEBUG( "Extract done in {}.", chrono.elapsedTimeStr() );

		molecule.forceNotifyDataChanged();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();
	}
	void GeneratedMolecule::extractAtom( const Chemistry::Atom & p_atom )
	{
		Util::Chrono chrono;
		chrono.start();

		Chemistry::Molecule & molecule = *p_atom.getMoleculePtr();
		_copyMoleculeData( molecule, ALL_FRAMES_INDEX, EXTRACT_PREFIX, EXTRACT_SUFFIX );

		const Chemistry::Chain & chain			= *p_atom.getChainPtr();
		Chemistry::Chain &		 generatedChain = addChain();
		_copyChainData( generatedChain, chain );
		generatedChain.setResidueCount( 1 );

		const Chemistry::Residue & residue			= *p_atom.getResiduePtr();
		Chemistry::Residue &	   generatedResidue = addResidue();
		_copyResidueData( generatedResidue, residue, &generatedChain );
		generatedResidue.setAtomCount( 1 );

		_extractAtomsFromResidue( molecule, &generatedResidue, p_atom.getIndex(), 1, false );

		_clearPendingExtractedBonds();
		_validateBuffers();

		molecule.forceNotifyDataChanged();
		molecule.refreshStructure();
		molecule.computeAllRepresentationData();

		chrono.stop();

		VTX_DEBUG( "Extract done in {}.", chrono.elapsedTimeStr() );
	}

	void GeneratedMolecule::_copyMoleculeData( const Chemistry::Molecule & p_molecule,
											   const int				   p_frame,
											   const std::string &		   p_namePrefix,
											   const std::string &		   p_nameSuffix )
	{
		// Copy molecule properties.
		setName( p_namePrefix + p_molecule.getName() + p_nameSuffix );
		setPdbIdCode( "none" );
		setDisplayName( p_namePrefix + p_molecule.getDefaultName() + p_nameSuffix );
		setColor( Util::Color::Rgba::randomPastel() );

		const App::Old::Application::Representation::InstantiatedRepresentation * const rep
			= App::Old::Application::Representation::RepresentationManager::get().instantiateCopy(
				p_molecule.getRepresentation(), *this, false, false );

		if ( p_frame == ALL_FRAMES_INDEX )
		{
			for ( int i = 0; i < p_molecule.getAtomPositionFrames().size(); i++ )
			{
				const AtomPositionsFrame & atomPosFrame			 = p_molecule.getAtomPositionFrames()[ i ];
				AtomPositionsFrame &	   generatedAtomPosFrame = addAtomPositionFrame();
				generatedAtomPosFrame.reserve( atomPosFrame.size() );
			}
		}
		else
		{
			const AtomPositionsFrame & atomPosFrame			 = p_molecule.getAtomPositionFrames()[ p_frame ];
			AtomPositionsFrame &	   generatedAtomPosFrame = addAtomPositionFrame();
			generatedAtomPosFrame.reserve( atomPosFrame.size() );
		}

		for ( const VTX::Core::ChemDB::UnknownResidueData * const unknownSymbolPtr :
			  p_molecule.getUnknownResidueSymbols() )
		{
			VTX::Core::ChemDB::UnknownResidueData * const unknownSymbolCopy
				= new VTX::Core::ChemDB::UnknownResidueData( *unknownSymbolPtr );
			addUnknownResidueSymbol( unknownSymbolCopy );
		}

		for ( const std::string & unknownSymbol : p_molecule.getUnknownAtomSymbols() )
			addUnknownAtomSymbol( unknownSymbol );

		getBufferAtomRadius().reserve( p_molecule.getAtomCount() );
		getBufferAtomIds().reserve( p_molecule.getAtomCount() );
		_transform = p_molecule.getTransform();

		setFPS( p_molecule.getFPS() );
		setPlayMode( p_molecule.getPlayMode() );
		const uint frame = Util::Math::min( getFrameCount() - 1, p_molecule.getFrame() );
		setFrame( frame );
		setIsPlaying( p_molecule.isPlaying() );
	}
	void GeneratedMolecule::_copyChainData( Chemistry::Chain & p_chain, const Chemistry::Chain & p_chainSource )
	{
		const App::Old::Application::Representation::InstantiatedRepresentation * const sourceRepresentation
			= p_chainSource.getRepresentation();

		const bool hasDefaultRepresentation
			= sourceRepresentation != nullptr
			  && p_chainSource.getMoleculePtr()->isDefaultRepresentation( *sourceRepresentation );

		p_chain.setMoleculePtr( this );
		p_chain.setIndex( getChainCount() - 1 );
		p_chain.setName( p_chainSource.getName() );
		p_chain.setOriginalChainID( p_chainSource.getOriginalChainID() );
		p_chain.setIndexFirstResidue( getResidueCount() );
		p_chain.setColor( Chemistry::Chain::getChainIdColor( p_chainSource.getOriginalChainID() ) );

		const ChemDB::Category::TYPE & chainCategoryEnum = p_chainSource.getCategoryEnum();
		p_chain.setCategoryEnum( chainCategoryEnum );

		if ( p_chainSource.hasCustomRepresentation() )
		{
			const App::Old::Application::Representation::InstantiatedRepresentation * const rep
				= App::Old::Application::Representation::RepresentationManager::get().instantiateCopy(
					p_chainSource.getRepresentation(), p_chain, false, false );

			if ( hasDefaultRepresentation )
				_markRepresentationAsDefault( rep );
		}
	}
	void GeneratedMolecule::_copyResidueData( Chemistry::Residue &		 p_residue,
											  const Chemistry::Residue & p_residueSource,
											  Chemistry::Chain * const	 p_parent )
	{
		p_residue.setIndex( getResidueCount() - 1 );
		p_residue.setChainPtr( p_parent );
		p_residue.setIndexInOriginalChain( p_residueSource.getIndexInOriginalChain() );
		p_residue.setIndexFirstAtom( getAtomCount() );
		p_residue.setSymbol( p_residueSource.getSymbol() );
		p_residue.setColor( Chemistry::Residue::getResidueColor( p_residueSource ) );
		p_residue.setType( p_residueSource.getType() );
		p_residue.setSecondaryStructure( p_residueSource.getSecondaryStructure() );
		p_residue.setAtomType( p_residueSource.getAtomType() );

		if ( p_residueSource.hasCustomRepresentation() )
		{
			App::Old::Application::Representation::RepresentationManager::get().instantiateCopy(
				p_residueSource.getRepresentation(), p_residue, false, false );
		}
	}
	void GeneratedMolecule::_copyAtomData( Chemistry::Atom &		  p_atom,
										   const Chemistry::Atom &	  p_atomSource,
										   Chemistry::Residue * const p_parent )
	{
		p_atom.setIndex( getAtomCount() - 1 );
		p_atom.setResiduePtr( p_parent );
		p_atom.setSymbol( p_atomSource.getSymbol() );
		p_atom.setName( p_atomSource.getName() );
		// p_atom.setColor( Chemistry::Atom::SYMBOL_COLOR[ int( p_atomSource.getSymbol() ) ] );
		p_atom.setType( p_atomSource.getType() );
		getBufferAtomRadius().emplace_back( p_atomSource.getVdwRadius() );
		getBufferAtomIds().emplace_back( p_atom.getId() );
		getBufferAtomIds().emplace_back( p_atom.getId() );
	}

	Chemistry::Chain & GeneratedMolecule::_extractFullChain( Chemistry::Molecule & p_fromMolecule, const uint p_index )
	{
		Chemistry::Chain &			   chain					 = *p_fromMolecule.getChain( p_index );
		const ChemDB::Category::TYPE & chainCategoryEnum		 = chain.getCategoryEnum();
		const uint					   previousFirstResidueIndex = chain.getIndexFirstResidue();
		const uint					   indexFirstResidue		 = getResidueCount();

		getMoleculeStruct().addChain( &chain.getChainStruct() );

		// Check contiguous atom ranges
		uint firstResidueIndex = previousFirstResidueIndex;

		uint	   firstAtomIndexRange = p_fromMolecule.getResidue( previousFirstResidueIndex )->getIndexFirstAtom();
		uint	   atomRangeCount	   = p_fromMolecule.getResidue( previousFirstResidueIndex )->getAtomCount();
		const uint lastResidueIndex	   = previousFirstResidueIndex + chain.getResidueCount() - 1;
		for ( uint i = previousFirstResidueIndex + 1; i <= lastResidueIndex; i++ )
		{
			const Chemistry::Residue & currResidue	  = *p_fromMolecule.getResidue( i );
			const uint				   indexFirstAtom = currResidue.getIndexFirstAtom();

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

		getCategory( chain.getCategoryEnum() ).addChain( chain.getIndex() );

		if ( chain.hasCustomRepresentation()
			 && p_fromMolecule.isDefaultRepresentation( *( chain.getRepresentation() ) ) )
		{
			App::Old::Application::Representation::InstantiatedRepresentation * const instantiatedRepresentation
				= App::Old::Application::Representation::RepresentationManager::get().instantiateCopy(
					chain.getRepresentation(), chain, false, false );

			_markRepresentationAsDefault( instantiatedRepresentation );
		}

		p_fromMolecule.removeChain( p_index, false, false, false );

		return chain;
	}

	void GeneratedMolecule::_extractAllResidues( Chemistry::Molecule & p_fromMolecule,
												 const uint			   p_startIndex,
												 const uint			   p_count )
	{
		const Chemistry::Residue * const firstSourceResidue	  = p_fromMolecule.getResidue( p_startIndex );
		const Chemistry::Residue * const lastSourceResidue	  = p_fromMolecule.getResidue( p_startIndex + p_count - 1 );
		const uint						 indexFirstSourceAtom = firstSourceResidue->getIndexFirstAtom();
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

			Chemistry::Residue & residue = *getResidue( currentResidueIndex );
			residue.setIndex( currentResidueIndex );
			residue.setRepresentableMolecule( this );
		}
	}

	Chemistry::Residue & GeneratedMolecule::_extractFullResidue( Chemistry::Molecule &	  p_fromMolecule,
																 const uint				  p_index,
																 Chemistry::Chain * const p_parent )
	{
		Chemistry::Residue & residue				= *p_fromMolecule.getResidue( p_index );
		const uint			 previousIndex			= residue.getIndex();
		const uint			 previousIndexFirstAtom = residue.getIndexFirstAtom();
		const uint			 indexFirstAtom			= getAtomCount();

		getResidues().emplace_back( &residue );
		_extractAtomsFromResidue( p_fromMolecule, &residue, previousIndexFirstAtom, residue.getAtomCount(), true );

		p_fromMolecule.removeResidue( previousIndex, false, false, true, false );
		residue.setIndex( getResidueCount() - 1 );
		residue.setChainPtr( p_parent );
		residue.setIndexFirstAtom( indexFirstAtom );

		return residue;
	}

	void GeneratedMolecule::_extractAllAtoms( Chemistry::Molecule & p_fromMolecule,
											  const uint			p_startIndex,
											  const uint			p_count )
	{
		const Chemistry::Residue * const previousFirstResidue = p_fromMolecule.getAtom( p_startIndex )->getResiduePtr();
		const Chemistry::Residue * const previousLastResidue
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

			const uint				fromBondIndex = bondOffset + i;
			Chemistry::Bond * const fromBond	  = p_fromMolecule.getBonds()[ bondOffset + i ];

			const bool bondFirstAtomInAtomRange = p_startIndex <= fromBond->getIndexFirstAtom()
												  && fromBond->getIndexFirstAtom() <= ( p_startIndex + p_count );

			const bool bondSecondAtomInAtomRange = p_startIndex <= fromBond->getIndexSecondAtom()
												   && fromBond->getIndexSecondAtom() <= ( p_startIndex + p_count );

			if ( !bondFirstAtomInAtomRange || !bondSecondAtomInAtomRange )
			{
				std::vector<ExternalBondExtractData>::iterator itExternalBond = _findInPendingExternalBond( *fromBond );

				if ( itExternalBond == _pendingExternalBonds.end() )
				{
					ExternalBondExtractData externalBondsData = ExternalBondExtractData( fromBond, i + bondOffset );

					if ( bondFirstAtomInAtomRange )
					{
						const uint newIndexFirstAtom = getBond( i )->getIndexFirstAtom() - atomOffset;
						externalBondsData.setFirstIndex( newIndexFirstAtom );
					}
					else if ( !bondSecondAtomInAtomRange )
					{
						const uint newIndexSecondAtom = getBond( i )->getIndexSecondAtom() - atomOffset;
						externalBondsData.setSecondIndex( newIndexSecondAtom );
					}

					externalBondsData.setIndexInExtractedResidue( i );
					_pendingExternalBonds.emplace_back( externalBondsData );
				}
				else
				{
					ExternalBondExtractData & pendingExternalBondData = *itExternalBond;

					if ( bondFirstAtomInAtomRange )
					{
						const uint newIndexFirstAtom = getBond( i )->getIndexFirstAtom() - atomOffset;
						pendingExternalBondData.setFirstIndex( newIndexFirstAtom );
					}
					else if ( bondSecondAtomInAtomRange )
					{
						const uint newIndexSecondAtom = getBond( i )->getIndexSecondAtom() - atomOffset;
						pendingExternalBondData.setSecondIndex( newIndexSecondAtom );
					}

					_applyExternalBond( pendingExternalBondData );
					_pendingExternalBonds.erase( itExternalBond );

					// Add second bond of external bonds
					_updateBondInfos(
						i, pendingExternalBondData.getFirstIndex(), pendingExternalBondData.getSecondIndex() );
				}
			}
			else
			{
				const Chemistry::Bond & bond = *( getBond( i ) );
				_updateBondInfos( i, bond.getIndexFirstAtom() - atomOffset, bond.getIndexSecondAtom() - atomOffset );
			}

			p_fromMolecule.getBonds()[ bondOffset + i ] = nullptr;
		}

		for ( uint i = previousFirstResidue->getIndex(); i <= previousLastResidue->getIndex(); i++ )
		{
			Chemistry::Residue * const residue = p_fromMolecule.getResidue( i );
			residue->setIndexFirstAtom( residue->getIndexFirstAtom() - atomOffset );
			residue->setIndexFirstBond( residue->getIndexFirstBond() - bondOffset );
		}

		for ( uint i = indexFirstAtom; i < getAtomCount(); i++ )
		{
			Chemistry::Atom & atom = *getAtom( i );
			atom.setIndex( i );
			getBufferAtomIds().emplace_back( atom.getId() );
		}

		for ( uint i = p_startIndex; i < p_startIndex + p_count; i++ )
			p_fromMolecule.removeAtom( i, false, false, false, false );
	}

	void GeneratedMolecule::_extractAtomsFromResidue( Chemistry::Molecule &		 p_fromMolecule,
													  Chemistry::Residue * const p_parent,
													  const uint				 p_startIndex,
													  const uint				 p_count,
													  bool						 p_parentFromMolecule )
	{
		std::vector<uint> vec = std::vector<uint>();
		vec.resize( p_count );
		for ( uint i = 0; i < p_count; i++ )
			vec[ i ] = p_startIndex + i;
		_extractAtomsFromResidue( p_fromMolecule, p_parent, vec, p_parentFromMolecule );
	}

	void GeneratedMolecule::_extractAtomsFromResidue( Chemistry::Molecule &		 p_fromMolecule,
													  Chemistry::Residue * const p_parent,
													  const std::vector<uint> &	 p_indexes,
													  bool						 p_parentFromMolecule )
	{
		const uint indexFirstAtom	 = getAtomCount();
		const uint newIndexFirstBond = getBondCount();

		const Chemistry::Residue & sourceResidue   = *p_fromMolecule.getAtom( p_indexes[ 0 ] )->getResiduePtr();
		const uint				   parentFirstAtom = sourceResidue.getIndexFirstAtom();
		const uint parentFirstAtomOut			   = sourceResidue.getIndexFirstAtom() + sourceResidue.getAtomCount();

		std::vector<BondExtractData> internalBonds = std::vector<BondExtractData>();
		internalBonds.reserve( sourceResidue.getBondCount() );

		std::vector<ExternalBondExtractData> externalBonds = std::vector<ExternalBondExtractData>();
		externalBonds.reserve( 4 );

		for ( const uint idAtom : p_indexes )
		{
			Chemistry::Atom &		   atom			= *p_fromMolecule.getAtom( idAtom );
			const Chemistry::Residue & fromResidue	= *atom.getResiduePtr();
			const uint				   newAtomIndex = getAtomCount();

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
				Chemistry::Bond * const bond = p_fromMolecule.getBond( idBond );

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
			Chemistry::Atom & atom = *getAtom( i );

			atom.setIndex( i );
			atom.setResiduePtr( p_parent );
		}

		// Update residue parent data
		p_parent->setIndexFirstBond( newIndexFirstBond );
		p_parent->setBondCount( getBondCount() - p_parent->getIndexFirstBond() );
	}

	void GeneratedMolecule::_updateBondInfos( const uint p_bondIndex,
											  const uint p_firstAtomIndex,
											  const uint p_secondAtomIndex )
	{
		Chemistry::Bond & p_bond = *( getBond( p_bondIndex ) );

		p_bond.setIndexFirstAtom( p_firstAtomIndex );
		p_bond.setIndexSecondAtom( p_secondAtomIndex );
		p_bond.setMoleculePtr( this );

		const uint bufferIndex				 = p_bondIndex * 2u;
		getBufferBonds()[ bufferIndex ]		 = p_firstAtomIndex;
		getBufferBonds()[ bufferIndex + 1u ] = p_secondAtomIndex;
	}
	void GeneratedMolecule::_extractBond( const BondExtractData & p_bondData )
	{
		Chemistry::Bond * const bond = p_bondData.getBond();

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
		const Chemistry::Bond & p_bond )
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

	std::string GeneratedMolecule::_getFrameCopyPrefix( const int p_frame ) const { return ""; }
	std::string GeneratedMolecule::_getFrameCopySuffix( const int p_frame ) const
	{
		return "[Frame " + std::to_string( p_frame ) + ']';
	}

} // namespace VTX::App::Old::Component::Chemistry