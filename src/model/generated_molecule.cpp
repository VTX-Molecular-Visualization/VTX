#include "generated_molecule.hpp"
#include "atom.hpp"
#include "chain.hpp"
#include "id.hpp"
#include "residue.hpp"
#include "tool/chrono.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		GeneratedMolecule::GeneratedMolecule() : Model::Molecule( ID::Model::MODEL_GENERATED_MOLECULE ) {}

		void GeneratedMolecule::copyFromSelection( const Model::Selection & _selection )
		{
			Tool::Chrono chrono;
			chrono.start();

			if ( _selection.getMoleculeSelectedCount() < 1 )
				throw Exception::VTXException( "Generate molecule from empty selection" );
			if ( _selection.getMoleculeSelectedCount() > 1 )
				throw Exception::VTXException( "Generate molecule from multiple molecule. Not allowed currently." );

			const std::pair<const ID, const Model::Selection::MapChainIds> & moleculeData = *( _selection.getItems().begin() );
			const Model::Molecule &											 molecule	  = MVC::MvcManager::get().getModel<Model::Molecule>( moleculeData.first );

			// Copy molecule properties.
			setName( "Extract from " + molecule.getName() );
			setPdbIdCode( "none" );
			setColor( Color::Rgb::randomPastel() );

			std::map<const uint, const uint> mapAtomIds = std::map<const uint, const uint>();

			for ( int i = 0; i < molecule.getAtomPositionFrames().size(); i++ )
			{
				const AtomPositionsFrame & atomPosFrame			 = molecule.getAtomPositionFrames()[ i ];
				AtomPositionsFrame &	   generatedAtomPosFrame = addAtomPositionFrame();
				generatedAtomPosFrame.reserve( atomPosFrame.size() );
			}

			for ( const std::string & unknownSymbol : molecule.getUnknownResidueSymbols() )
				addUnknownResidueSymbol( unknownSymbol );
			for ( const std::string & unknownSymbol : molecule.getUnknownAtomSymbols() )
				addUnknownAtomSymbol( unknownSymbol );

			getBufferAtomRadius().reserve( molecule.getAtomCount() );

			for ( const std::pair<const ID, const Model::Selection::MapResidueIds> & chainData : moleculeData.second )
			{
				const Model::Chain & chain			= molecule.getChain( chainData.first );
				Model::Chain &		 generatedChain = addChain();
				generatedChain.setMoleculePtr( this );
				generatedChain.setIndex( getChainCount() - 1 );
				generatedChain.setName( chain.getName() );
				generatedChain.setIndexFirstResidue( getResidueCount() );
				generatedChain.setColor( Model::Chain::getChainIdColor( chain.getIndex() ) );
				generatedChain.setResidueCount( uint( chainData.second.size() ) );

				for ( const std::pair<const ID, const Model::Selection::VecAtomIds> & residueData : chainData.second )
				{
					const Model::Residue & residue			= molecule.getResidue( residueData.first );
					Model::Residue &	   generatedResidue = addResidue();

					generatedResidue.setIndex( getResidueCount() - 1 );
					generatedResidue.setIndexInOriginalChain( residue.getIndexInOriginalChain() );
					generatedResidue.setMoleculePtr( this );
					generatedResidue.setChainPtr( &generatedChain );
					generatedResidue.setIndexFirstAtom( getAtomCount() );
					generatedResidue.setAtomCount( uint( residueData.second.size() ) );
					generatedResidue.setSymbol( residue.getSymbol() );
					generatedResidue.setColor( Model::Residue::SYMBOL_COLOR[ int( residue.getSymbol() ) ] );
					generatedResidue.setType( residue.getType() );
					generatedResidue.setSecondaryStructure( residue.getSecondaryStructure() );
					generatedResidue.setAtomType( residue.getAtomType() );

					for ( const ID & atomId : residueData.second )
					{
						const Model::Atom & atom = molecule.getAtom( atomId );

						Model::Atom & generatedAtom		 = addAtom();
						const uint	  generatedAtomIndex = getAtomCount() - 1;
						generatedAtom.setIndex( generatedAtomIndex );

						mapAtomIds.emplace( atom.getIndex(), generatedAtomIndex );

						generatedAtom.setMoleculePtr( this );
						generatedAtom.setChainPtr( &generatedChain );
						generatedAtom.setResiduePtr( &generatedResidue );
						generatedAtom.setSymbol( atom.getSymbol() );
						generatedAtom.setName( atom.getName() );
						generatedAtom.setColor( Model::Atom::SYMBOL_COLOR[ int( atom.getSymbol() ) ] );
						generatedAtom.setType( atom.getType() );
						getBufferAtomRadius().emplace_back( generatedAtom.getVdwRadius() );

						// Copy atom position for each frame
						for ( int i = 0; i < getAtomPositionFrames().size(); i++ )
							getAtomPositionFrame( i ).emplace_back( molecule.getAtomPositionFrame( i )[ atom.getIndex() ] );
					}
				}
			}

			// Bonds
			Model::Residue * previousResidue = nullptr;
			uint			 counter		 = 0;
			getBufferBonds().reserve( molecule.getBufferBonds().size() );
			for ( uint i = 0; i < molecule.getBondCount(); i++ )
			{
				const Model::Bond & bond = *molecule.getBonds()[ i ];

				if ( mapAtomIds.find( bond.getIndexFirstAtom() ) != mapAtomIds.end() && mapAtomIds.find( bond.getIndexSecondAtom() ) != mapAtomIds.end() )
				{
					const uint indexFirstAtom  = mapAtomIds[ bond.getIndexFirstAtom() ];
					const uint indexSecondAtom = mapAtomIds[ bond.getIndexSecondAtom() ];

					Model::Bond & generatedBond = addBond();

					generatedBond.setIndexFirstAtom( indexFirstAtom );
					generatedBond.setIndexSecondAtom( indexSecondAtom );

					getBufferBonds().emplace_back( indexFirstAtom );
					getBufferBonds().emplace_back( indexSecondAtom );

					const uint generatedBondIndex = getBondCount() - 1;

					Model::Residue * const startResidue = getAtom( indexFirstAtom ).getResiduePtr();
					Model::Residue * const endResidue	= getAtom( indexSecondAtom ).getResiduePtr();
					const bool			   extraBound	= startResidue != endResidue;

					if ( extraBound )
					{
						startResidue->getIndexExtraBondStart().emplace_back( generatedBondIndex );
						endResidue->getIndexExtraBondEnd().emplace_back( generatedBondIndex );
					}
					else
					{
						if ( getAtom( indexFirstAtom ).getResiduePtr() != previousResidue )
						{
							if ( previousResidue != nullptr )
							{
								previousResidue->setBondCount( counter );
							}
							getAtom( indexFirstAtom ).getResiduePtr()->setIndexFirstBond( generatedBondIndex );
							previousResidue = getAtom( indexFirstAtom ).getResiduePtr();

							counter = 1;
						}
						else
						{
							counter++;
						}
					}
				}
			}

			if ( previousResidue != nullptr )
				previousResidue->setBondCount( counter );

			getBufferBonds().shrink_to_fit();

			getBufferAtomVisibilities().resize( getAtomCount(), 1u );
			getBufferAtomSelection().resize( getAtomCount(), 0u );

			getBufferAtomRadius().shrink_to_fit();
			for ( AtomPositionsFrame & atomPositions : getAtomPositionFrames() )
				atomPositions.shrink_to_fit();

			VTX_INFO( "Copy done in : " + std::to_string( chrono.elapsedTime() ) + "s" );

			chrono.stop();
		}
	} // namespace Model
} // namespace VTX
