#include "molecule.hpp"
#include "color/rgb.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "id.hpp"
#include "model/atom.hpp"
#include "model/bond.hpp"
#include "model/chain.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/secondary_structure.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include "util/secondary_structure.hpp"
#include "view/d3/box.hpp"
#include "view/d3/cylinder.hpp"
#include "view/d3/sphere.hpp"
#include "vtx_app.hpp"
#include <algorithm>

namespace VTX
{
	namespace Model
	{
		Molecule::Molecule() : Molecule( ID::Model::MODEL_MOLECULE ) {}
		Molecule::Molecule( const ID::VTX_ID & p_typeId ) : BaseModel3D( ID::Model::MODEL_MOLECULE ) {}

		Molecule::~Molecule()
		{
			MVC::MvcManager::get().deleteAllModels( _atoms );
			MVC::MvcManager::get().deleteAllModels( _bonds );
			MVC::MvcManager::get().deleteAllModels( _residues );
			MVC::MvcManager::get().deleteAllModels( _chains );

			if ( _secondaryStructure != nullptr )
				MVC::MvcManager::get().deleteModel( _secondaryStructure );
		}

		void Molecule::setPdbIdCode( const std::string & p_pdbId )
		{
			_pdbIdCode = p_pdbId;
			setDisplayName( _pdbIdCode );
		}

		Chain & Molecule::addChain()
		{
			Chain * const chain = MVC::MvcManager::get().instantiateModel<Chain>();
			_chains.emplace_back( chain );
			_realChainCount++;
			return *chain;
		}

		Residue & Molecule::addResidue()
		{
			Residue * const residue = MVC::MvcManager::get().instantiateModel<Residue>();
			_residues.emplace_back( residue );
			return *residue;
		}

		Atom & Molecule::addAtom()
		{
			Atom * const atom = MVC::MvcManager::get().instantiateModel<Atom>();
			_atoms.emplace_back( atom );
			return *atom;
		}

		Bond & Molecule::addBond()
		{
			Bond * const bond = MVC::MvcManager::get().instantiateModel<Bond>();
			_bonds.emplace_back( bond );
			return *bond;
		}

		void Molecule::_init()
		{
			BaseModel::setDefaultName( &_displayName );

			// Fill buffers.
			if ( _atomPositionsFrames.size() > 0 )
			{
				// Compute secondary structure if not loaded.
				if ( _configuration.isSecondaryStructureLoadedFromFile == false )
				{
					Util::SecondaryStructure::computeSecondaryStructure( *this );
				}

				// Create secondary structure mesh.
				createSecondaryStructure();

				setRepresentableMolecule( this );

				if ( hasCustomRepresentation() )
				{
					computeAllRepresentationData();
				}
				else
				{
					applyDefaultRepresentation();
				}
			}
		}

		bool Molecule::isEmpty()
		{
			for ( uint i = 0; i < getChainCount(); i++ )
			{
				if ( _chains[ i ] != nullptr )
				{
					return false;
				}
			}

			return true;
		}

		void Molecule::applyRepresentation(
			Generic::BaseRepresentable::InstantiatedRepresentation * const p_representation )
		{
			BaseRepresentable::applyRepresentation( p_representation );
			_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
		}
		void Molecule::removeRepresentation()
		{
			BaseRepresentable::removeRepresentation();
			_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
		}
		void Molecule::removeChildrenRepresentations() const
		{
			for ( Model::Chain * const chain : _chains )
			{
				if ( chain == nullptr )
					continue;

				chain->removeRepresentation();
				chain->removeChildrenRepresentations();
			}
		}

		void Molecule::_fillBuffer()
		{
			_buffer->setAtomPositions( _atomPositionsFrames[ _currentFrame ] );
			_buffer->setAtomRadius( _bufferAtomRadius );
			_fillBufferAtomColors();
			_buffer->setAtomVisibilities( _bufferAtomVisibilities );
			_buffer->setAtomSelections( _bufferAtomSelection );
			_buffer->setBonds( _bufferBonds );
		}

		void Molecule::_computeAABB() const
		{
			for ( const Model::Atom * const atom : _atoms )
			{
				if ( atom == nullptr )
					continue;

				const uint	atomIndex  = atom->getIndex();
				const float atomRadius = atom->getVdwRadius();
				for ( const AtomPositionsFrame & frame : _atomPositionsFrames )
				{
					_aabb.extend( frame[ atomIndex ], atomRadius );
				}
			}
		}

		void Molecule::_fillBufferAABB()
		{
			uint counter	   = 0;
			_bufferAABBCorners = std::vector<Vec3f>();
			_bufferAABBIndices = std::vector<uint>();
			for ( const Residue * const elem : _residues )
			{
				const Math::AABB & aabb = elem->getWorldAABB();

				const Vec3f & min = aabb.getMin();
				const Vec3f & max = aabb.getMax();

				_bufferAABBCorners.insert( _bufferAABBCorners.end(),
										   { min,
											 Vec3f( max.x, min.y, min.z ),
											 Vec3f( max.x, max.y, min.z ),
											 Vec3f( min.x, max.y, min.z ),
											 Vec3f( min.x, min.y, max.z ),
											 Vec3f( max.x, min.y, max.z ),
											 max,
											 Vec3f( min.x, max.y, max.z ) } );

				_bufferAABBIndices.insert(
					_bufferAABBIndices.end(),
					{ counter + 0, counter + 1, counter + 1, counter + 2, counter + 2, counter + 3,
					  counter + 3, counter + 0, counter + 4, counter + 5, counter + 5, counter + 6,
					  counter + 6, counter + 7, counter + 7, counter + 4, counter + 0, counter + 4,
					  counter + 1, counter + 5, counter + 2, counter + 6, counter + 3, counter + 7 } );

				counter += 8u;
			}

			_buffer->setAABBCorners( _bufferAABBCorners );
			_buffer->setAABBIndices( _bufferAABBIndices );
		}

		void Molecule::_instantiate3DViews()
		{
			//_viewBox = MVC::MvcManager::get().instantiateView<View::D3::Box>( this, ID::View::D3_BOX );
			//_viewBox->init();

			_addRenderable( MVC::MvcManager::get().instantiateView<View::D3::Sphere>( this, ID::View::D3_SPHERE ) );
			_addRenderable( MVC::MvcManager::get().instantiateView<View::D3::Cylinder>( this, ID::View::D3_CYLINDER ) );
		}

		void Molecule::refreshBondsBuffer()
		{
			_buffer->setBonds( _bufferBonds );
			computeRepresentationTargets();
		}

		void Molecule::_fillBufferAtomColors()
		{
			_bufferAtomColors.resize( _atoms.size() );
			computeColorBuffer();
		}

		void Molecule::_fillBufferAtomVisibilities()
		{
			_bufferAtomVisibilities.clear();
			_bufferAtomVisibilities.resize( _atoms.size(), 1u );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				const Atom * const atom = _atoms[ i ];
				// Solvent hidden.
				if ( _showSolvent == false && atom->getType() == Atom::TYPE::SOLVENT )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				// Ion hidden.
				else if ( _showIon == false && atom->getType() == Atom::TYPE::ION )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
			}

			_buffer->setAtomVisibilities( _bufferAtomVisibilities );
		}

		void Molecule::_fillBufferAtomSelections( const Model::Selection::MapChainIds * const p_selection )
		{
			_bufferAtomSelection.clear();
			_bufferAtomSelection.resize( _atoms.size(), 0u );
			if ( p_selection != nullptr )
			{
				for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : *p_selection )
				{
					for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
					{
						for ( const uint & atomIndex : pairResidue.second )
						{
							_bufferAtomSelection[ atomIndex ] = 1u;
						}
					}
				}
			}

			_buffer->setAtomSelections( _bufferAtomSelection );
		}

		void Molecule::refreshSelection( const Model::Selection::MapChainIds * const p_selection )
		{
			_fillBufferAtomSelections( p_selection );
			_secondaryStructure->refreshSelection( p_selection );
		}

		void Molecule::setFrame( const uint p_frameIdx )
		{
			if ( p_frameIdx > getFrameCount() )
			{
				VTX_WARNING( "Frame " + std::to_string( p_frameIdx )
							 + " does not exists / Count: " + std::to_string( getFrameCount() ) );
				return;
			}

			_currentFrame = p_frameIdx;
			_buffer->setAtomPositions( _atomPositionsFrames[ _currentFrame ] );
			//_secondaryStructure->setCurrentFrame();
		}

		void Molecule::print() const
		{
			// TODO: add more infos in debug (solvents, ions, ss...).
			VTX_INFO( "Molecule: " + _name );
			VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: "
					  + std::to_string( _residues.size() ) + " / Atoms: " + std::to_string( _atoms.size() )
					  + " / Bonds: " + std::to_string( _bonds.size() ) );

			// Display unknown symbols.
			const std::unordered_set<std::string> & unknownResidueSymbols = getUnknownResidueSymbols();
			if ( unknownResidueSymbols.empty() == false )
			{
				std::string unknownResidueSymbolsStr = "";
				for ( std::string symbol : unknownResidueSymbols )
				{
					unknownResidueSymbolsStr += symbol + " ";
				}
				VTX_WARNING( "Unknown residue symbols : " + unknownResidueSymbolsStr );
			}

			const std::unordered_set<std::string> & unknownAtomSymbols = getUnknownAtomSymbols();
			if ( unknownAtomSymbols.empty() == false )
			{
				std::string unknownAtomSymbolsStr = "";
				for ( std::string symbol : unknownAtomSymbols )
				{
					unknownAtomSymbolsStr += symbol + " ";
				}
				VTX_WARNING( "Unknown atom symbols : " + unknownAtomSymbolsStr );
			}

			VTX_DEBUG( "Sizeof molecule: " + std::to_string( sizeof( *this ) ) );
			VTX_DEBUG( "Sizeof chain: " + std::to_string( sizeof( *_chains[ 0 ] ) ) );
			VTX_DEBUG( "Sizeof residue: " + std::to_string( sizeof( *_residues[ 0 ] ) ) );
			VTX_DEBUG( "Sizeof atom: " + std::to_string( sizeof( *_atoms[ 0 ] ) ) );
			VTX_DEBUG( "Sizeof bond: " + std::to_string( sizeof( *_bonds[ 0 ] ) ) );
		}

		void Molecule::render( const Object3D::Camera & p_camera )
		{
			BaseModel3D::render( p_camera );
			_secondaryStructure->render( p_camera );
		}

		bool Molecule::mergeTopology( const Molecule & p_molecule )
		{
			if ( p_molecule.getAtomCount() > getAtomCount() )
			{
				VTX_ERROR( "Too many atoms in topology model" );
				return false;
			}
			else if ( getAtomCount() > p_molecule.getAtomCount() )
			{
				VTX_WARNING( "Topology model has less atoms than dynamic one (maybe solvent not present)" );
			}

			// Clear topology.
			MVC::MvcManager::get().deleteAllModels( _chains );
			MVC::MvcManager::get().deleteAllModels( _residues );

			// Create models.
			_chains.resize( p_molecule.getChainCount() );
			for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
			{
				getChains()[ i ] = MVC::MvcManager::get().instantiateModel<Chain>();
			}
			_residues.resize( p_molecule.getResidueCount() );
			for ( uint i = 0; i < p_molecule.getResidueCount(); ++i )
			{
				getResidues()[ i ] = MVC::MvcManager::get().instantiateModel<Residue>();
			}

			setName( p_molecule.getName() );
			// Loop over other molecule chains.
			for ( uint chainIndex = 0; chainIndex < p_molecule.getChainCount(); ++chainIndex )
			{
				const Chain &  otherChain = *p_molecule.getChain( chainIndex );
				Model::Chain & modelChain = *getChain( chainIndex );
				modelChain.setIndex( otherChain.getIndex() );
				modelChain.setMoleculePtr( this );
				modelChain.setName( otherChain.getName() );
				modelChain.setIndexFirstResidue( otherChain.getIndexFirstResidue() );
				modelChain.setResidueCount( otherChain.getResidueCount() );

				for ( uint residueIndex = 0; residueIndex < otherChain.getResidueCount(); ++residueIndex )
				{
					const Residue * const otherResidue
						= p_molecule.getResidue( otherChain.getIndexFirstResidue() + residueIndex );
					Model::Residue * const modelResidue
						= getResidue( otherChain.getIndexFirstResidue() + residueIndex );

					modelResidue->setIndex( otherResidue->getIndex() );
					modelResidue->setChainPtr( &modelChain );
					modelResidue->setIndexFirstAtom( otherResidue->getIndexFirstAtom() );
					modelResidue->setAtomCount( otherResidue->getAtomCount() );
					modelResidue->setSymbol( otherResidue->getSymbol() );

					for ( uint atomIndex = 0; atomIndex < otherResidue->getAtomCount(); ++atomIndex )
					{
						const Atom &  otherAtom = *p_molecule.getAtom( otherResidue->getIndexFirstAtom() + atomIndex );
						Model::Atom & modelAtom = *getAtom( otherResidue->getIndexFirstAtom() + atomIndex );
						modelAtom.setIndex( otherAtom.getIndex() );
						modelAtom.setResiduePtr( modelResidue );
					}
				}
			}

			// Loop over other atoms.
			if ( getAtomCount() > p_molecule.getAtomCount() )
			{
				addChain();
				addResidue();
				Model::Chain & modelChain	= *getChain( getChainCount() - 1 );
				Model::Residue modelResidue = *getResidue( getResidueCount() - 1 );
				modelChain.setIndex( getChainCount() - 1 );
				modelChain.setIndexFirstResidue( getResidueCount() - 1 );
				modelChain.setResidueCount( 1 );
				modelResidue.setIndex( getResidueCount() - 1 );
				modelResidue.setChainPtr( &modelChain );
				modelResidue.setIndexFirstAtom( p_molecule.getAtomCount() );
				modelResidue.setAtomCount( getAtomCount() - p_molecule.getAtomCount() );

				for ( uint atomIndex = 0; atomIndex < modelResidue.getAtomCount(); ++atomIndex )
				{
					Model::Atom & modelAtom = *getAtom( modelResidue.getIndexFirstAtom() + atomIndex );
					modelAtom.setResiduePtr( &modelResidue );
				}
			}

			return true;
		}

		void Molecule::createSecondaryStructure()
		{
			if ( _secondaryStructure != nullptr )
			{
				MVC::MvcManager::get().deleteModel( _secondaryStructure );
			}

			_secondaryStructure = MVC::MvcManager::get().instantiateModel<SecondaryStructure, Molecule * const>( this );
			_secondaryStructure->init( getBuffer()->gl() );
			_secondaryStructure->print();
		}

		void Molecule::refreshSecondaryStructure()
		{
			if ( _secondaryStructure == nullptr )
			{
				VTX_ERROR( "No secondary structure" );
				return;
			}

			_secondaryStructure->refresh();
		}

		void Molecule::setVisible( const bool p_visible )
		{
			if ( isVisible() != p_visible )
			{
				BaseVisible::setVisible( p_visible );
				_notifyViews( new Event::VTXEvent( Event::Model::MOLECULE_VISIBILITY ) );
			}
		}

		const Chain * const Molecule::getPreviousChain( const uint p_idBaseChain ) const
		{
			if ( p_idBaseChain == 0 )
				return nullptr;

			for ( uint i = p_idBaseChain - 1; i > 0; i-- )
				if ( _chains[ i ] != nullptr )
					return _chains[ i ];

			return _chains[ 0 ];
		}
		Chain * const Molecule::getPreviousChain( const uint p_idBaseChain )
		{
			if ( p_idBaseChain == 0 )
				return nullptr;

			for ( uint i = p_idBaseChain - 1; i > 0; i-- )
				if ( _chains[ i ] != nullptr )
					return _chains[ i ];

			return _chains[ 0 ];
		}
		const Chain * const Molecule::getNextChain( const uint p_idBaseChain ) const
		{
			for ( uint i = p_idBaseChain + 1; i < _chains.size(); i++ )
				if ( _chains[ i ] != nullptr )
					return _chains[ i ];
			return nullptr;
		}
		Chain * const Molecule::getNextChain( const uint p_idBaseChain )
		{
			for ( uint i = p_idBaseChain + 1; i < _chains.size(); i++ )
				if ( _chains[ i ] != nullptr )
					return _chains[ i ];
			return nullptr;
		}

		const Residue * const Molecule::getPreviousResidue( const uint p_idBaseResidue ) const
		{
			if ( p_idBaseResidue == 0 )
				return nullptr;

			for ( uint i = p_idBaseResidue - 1; i > 0; i-- )
				if ( _residues[ i ] != nullptr )
					return _residues[ i ];

			return _residues[ 0 ];
		}
		Residue * const Molecule::getPreviousResidue( const uint p_idBaseResidue )
		{
			if ( p_idBaseResidue == 0 )
				return nullptr;

			for ( uint i = p_idBaseResidue - 1; i > 0; i-- )
				if ( _residues[ i ] != nullptr )
					return _residues[ i ];

			return _residues[ 0 ];
		}
		const Residue * const Molecule::getNextResidue( const uint p_idBaseResidue ) const
		{
			for ( uint i = p_idBaseResidue + 1; i < _residues.size(); i++ )
				if ( _residues[ i ] != nullptr )
					return _residues[ i ];
			return nullptr;
		}
		Residue * const Molecule::getNextResidue( const uint p_idBaseResidue )
		{
			for ( uint i = p_idBaseResidue + 1; i < _residues.size(); i++ )
				if ( _residues[ i ] != nullptr )
					return _residues[ i ];
			return nullptr;
		}

		void Molecule::removeChain( const uint p_id,
									const bool p_delete,
									const bool p_recursive,
									const bool p_notifyViews )
		{
			if ( p_recursive )
			{
				// Delete Residues
				for ( uint residueIndex = _chains[ p_id ]->getIndexFirstResidue();
					  residueIndex < ( _chains[ p_id ]->getIndexFirstResidue() + _chains[ p_id ]->getResidueCount() );
					  residueIndex++ )
				{
					removeResidue( residueIndex, p_delete, p_recursive, false, false );
				}
			}

			// Delete chain
			if ( p_delete )
				MVC::MvcManager::get().deleteModel( _chains[ p_id ] );

			_chains[ p_id ] = nullptr;
			_realChainCount--;

			_aabb.invalidate();

			// Notify
			if ( p_notifyViews )
				_notifyDataChanged();
		}

		void Molecule::removeResidue( const uint p_id,
									  const bool p_delete,
									  const bool p_recursive,
									  const bool p_checkParentUpdate,
									  const bool p_notifyViews )
		{
			// Delete Atoms
			if ( p_recursive )
			{
				for ( uint atomIndex = _residues[ p_id ]->getIndexFirstAtom();
					  atomIndex < _residues[ p_id ]->getIndexFirstAtom() + _residues[ p_id ]->getAtomCount();
					  atomIndex++ )
				{
					if ( _atoms[ atomIndex ] == nullptr )
						continue;

					removeAtom( atomIndex, p_delete, true, false, false );
				}
			}

			// Register parent before nullify
			Model::Chain * const parent = _residues[ p_id ]->getChainPtr();

			// Remove Residue
			if ( p_delete )
				MVC::MvcManager::get().deleteModel( _residues[ p_id ] );

			_residues[ p_id ] = nullptr;

			// Update parent indexes
			if ( p_checkParentUpdate )
			{
				parent->removeToResidues( p_id );

				if ( parent->getResidueCount() == 0 )
					removeChain( parent->getIndex(), p_delete, false, false );
			}

			if ( p_checkParentUpdate && parent->getResidueCount() == 0 )
			{
				removeChain( parent->getIndex(), p_delete, true, p_notifyViews );
			}
			else
			{
				_aabb.invalidate();

				// Notify
				if ( p_notifyViews )
					_notifyDataChanged();
			}
		}

		void Molecule::removeAtom( const uint p_id,
								   const bool p_delete,
								   const bool p_checkInternalBonds,
								   const bool p_checkParentUpdate,
								   const bool p_notifyViews )
		{
			Model::Residue * const parent = _atoms[ p_id ]->getResiduePtr();

			// Delete Bonds
			if ( p_checkInternalBonds )
			{
				for ( uint bondIndex = parent->getIndexFirstBond();
					  bondIndex < parent->getIndexFirstBond() + parent->getBondCount();
					  bondIndex++ )
				{
					const Bond * const bond = _bonds[ bondIndex ];
					if ( bond != nullptr
						 && ( bond->getIndexFirstAtom() == p_id || bond->getIndexSecondAtom() == p_id ) )
						removeBond( bondIndex, p_delete, false );
				}
			}

			for ( const uint & bondId : parent->getIndexExtraBondStart() )
			{
				const Bond * const bond = _bonds[ bondId ];
				if ( bond != nullptr && bond->getIndexFirstAtom() == p_id )
				{
					Model::Residue * const other			  = getAtom( bond->getIndexSecondAtom() )->getResiduePtr();
					std::vector<uint> &	   otherExtraResidues = other->getIndexExtraBondEnd();

					removeBond( bondId, p_delete, false );

					otherExtraResidues.erase(
						std::find( otherExtraResidues.begin(), otherExtraResidues.end(), bondId ) );
				}
			}
			for ( const uint & bondId : parent->getIndexExtraBondEnd() )
			{
				const Bond * const bond = _bonds[ bondId ];
				if ( bond != nullptr && bond->getIndexSecondAtom() == p_id )
				{
					Model::Residue * const other			  = getAtom( bond->getIndexFirstAtom() )->getResiduePtr();
					std::vector<uint> &	   otherExtraResidues = other->getIndexExtraBondStart();

					removeBond( bondId, p_delete, false );

					otherExtraResidues.erase(
						std::find( otherExtraResidues.begin(), otherExtraResidues.end(), bondId ) );
				}
			}

			// Delete Atom
			if ( p_delete )
				MVC::MvcManager::get().deleteModel( _atoms[ p_id ] );

			_bufferAtomVisibilities[ p_id ] = 0u;
			_atoms[ p_id ]					= nullptr;

			// Update parent indexes
			if ( p_checkParentUpdate )
			{
				parent->removeToAtoms( p_id );
			}

			if ( p_checkParentUpdate && parent->getAtomCount() == 0 )
			{
				removeResidue( parent->getIndex(), p_delete, true, p_checkParentUpdate, p_notifyViews );
			}
			else
			{
				_aabb.invalidate();

				// Notify
				if ( p_notifyViews )
					_notifyDataChanged();
			}
		}

		void Molecule::removeBond( const uint p_id, const bool p_delete, const bool p_notifyViews )
		{
			if ( p_delete )
				MVC::MvcManager::get().deleteModel( _bonds[ p_id ] );

			_bonds[ p_id ] = nullptr;

			_bufferBonds[ p_id * 2u ]	   = 0;
			_bufferBonds[ p_id * 2u + 1u ] = 0;

			if ( p_notifyViews )
				_notifyDataChanged();
		}

		void Molecule::setDisplayName( const std::string & p_name )
		{
			_displayName = p_name;
			_notifyViews( new Event::VTXEvent( Event::Model::DISPLAY_NAME_CHANGE ) );
		}

		void Molecule::setColor( const Color::Rgb & p_color )
		{
			Generic::BaseColorable::setColor( p_color );
			VTX_EVENT( new Event::VTXEventRef( Event::Global::MOLECULE_COLOR_CHANGE, p_color ) );
		}

	} // namespace Model
} // namespace VTX
