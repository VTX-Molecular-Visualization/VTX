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
#include "representation/representation_manager.hpp"
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
		Molecule::Molecule( const ID::VTX_ID & p_typeId ) : BaseModel3D( ID::Model::MODEL_MOLECULE )
		{
			_playMode = VTX_SETTING().getDefaultTrajectoryPlayMode();
			_fps	  = VTX_SETTING().getDefaultTrajectorySpeed();
		}

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
		}

		Chain & Molecule::addChain()
		{
			Chain * const chain = MVC::MvcManager::get().instantiateModel<Chain>();
			_addChain( chain );
			return *chain;
		}
		void Molecule::_addChain( Model::Chain * const p_chain )
		{
			_chains.emplace_back( p_chain );
			_realChainCount++;
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
					VTX::Representation::RepresentationManager::get().instantiateDefaultRepresentation( *this );
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

		void Molecule::removeRepresentation( const bool p_notify )
		{
			BaseRepresentable::removeRepresentation( false );

			VTX::Representation::RepresentationManager::get().instantiateDefaultRepresentation( *this );

			if ( p_notify )
				_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
		}
		void Molecule::removeChildrenRepresentations() const
		{
			for ( Model::Chain * const chain : _chains )
			{
				if ( chain == nullptr )
					continue;

				VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
					*chain, false, false );

				chain->removeChildrenRepresentations();
			}
		}
		void Molecule::_onRepresentationChange()
		{
			_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_CHANGE ) );
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

		void Molecule::refreshBondsBuffer() { _buffer->setBonds( _bufferBonds ); }

		void Molecule::_fillBufferAtomColors()
		{
			_bufferAtomColors.resize( _atoms.size() );

			for ( const Model::Residue * const residue : getResidues() )
			{
				// Skip hidden items.
				if ( residue == nullptr )
					continue;

				const Model::Representation::InstantiatedRepresentation * const currentRepresentation
					= residue->getRepresentation();

				Generic::COLOR_MODE colorMode = currentRepresentation->getColorMode();

				if ( colorMode == Generic::COLOR_MODE::INHERITED )
				{
					const Generic::COLOR_MODE & chainColorMode
						= residue->getChainPtr()->getRepresentation()->getColorMode();
					if ( chainColorMode != Generic::COLOR_MODE::INHERITED )
					{
						colorMode = chainColorMode;
					}
					else
					{
						const Generic::COLOR_MODE & moleculeColorMode = getRepresentation()->getColorMode();
						if ( moleculeColorMode != Generic::COLOR_MODE::INHERITED )
							colorMode = moleculeColorMode;
						else
							colorMode = Setting::COLOR_MODE_DEFAULT;
					}
				}

				bool	   colorCarbon = false;
				Color::Rgb color;

				switch ( colorMode )
				{
				case Generic::COLOR_MODE::ATOM_CHAIN: colorCarbon = true; [[fallthrough]];
				case Generic::COLOR_MODE::CHAIN: color = residue->getChainPtr()->getColor(); break;

				case Generic::COLOR_MODE::ATOM_PROTEIN: colorCarbon = true; [[fallthrough]];
				case Generic::COLOR_MODE::PROTEIN: color = getColor(); break;

				case Generic::COLOR_MODE::ATOM_CUSTOM: colorCarbon = true; [[fallthrough]];
				case Generic::COLOR_MODE::CUSTOM: color = currentRepresentation->getColor(); break;

				case Generic::COLOR_MODE::RESIDUE:
					colorCarbon = false;
					color		= residue->getColor();
					break;
				}

				for ( uint i = residue->getIndexFirstAtom(); i < residue->getIndexFirstAtom() + residue->getAtomCount();
					  i++ )
				{
					const Model::Atom * const atom = getAtom( i );

					if ( atom == nullptr )
						continue;

					if ( colorCarbon && atom->getSymbol() != Model::Atom::SYMBOL::A_C )
					{
						_bufferAtomColors[ i ] = atom->getColor();
					}
					else
					{
						_bufferAtomColors[ i ] = color;
					}
				}
			}

			_buffer->setAtomColors( _bufferAtomColors );
		}

		void Molecule::_fillBufferAtomVisibilities()
		{
			_bufferAtomVisibilities.clear();

			if ( isVisible() == false )
			{
				_bufferAtomVisibilities.resize( _atoms.size(), 0u );
			}
			else
			{
				_bufferAtomVisibilities.resize( _atoms.size(), 1u );

				for ( uint i = 0; i < uint( _atoms.size() ); ++i )
				{
					const Atom * const atom = _atoms[ i ];

					if ( atom == nullptr )
					{
						_bufferAtomVisibilities[ i ] = 0u;
						continue;
					}

					if ( atom->getChainPtr()->isVisible() == false )
					{
						_bufferAtomVisibilities[ i ] = 0u;
					}
					else if ( atom->getResiduePtr()->isVisible() == false )
					{
						_bufferAtomVisibilities[ i ] = 0u;
					}
					else if ( atom->isVisible() == false )
					{
						_bufferAtomVisibilities[ i ] = 0u;
					}
					// Solvent hidden.
					else if ( _showSolvent == false && atom->getType() == Atom::TYPE::SOLVENT )
					{
						_bufferAtomVisibilities[ i ] = 0u;
					}
					// Ion hidden.
					else if ( _showIon == false && atom->getType() == Atom::TYPE::ION )
					{
						_bufferAtomVisibilities[ i ] = 0u;
					}
					else if ( _showHydrogen == false && atom->getSymbol() == Atom::SYMBOL::A_H )
					{
						_bufferAtomVisibilities[ i ] = 0u;
					}
					else if ( _showWater == false
							  && ( atom->getResiduePtr()->getSymbol() == Model::Residue::SYMBOL::HOH
								   || atom->getResiduePtr()->getSymbol() == Model::Residue::SYMBOL::WAT ) )
					{
						_bufferAtomVisibilities[ i ] = 0u;
					}
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
			_secondaryStructure->refresh( true );

			_notifyViews( new Event::VTXEvent( Event::Model::TRAJECTORY_FRAME_CHANGE ) );

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}
		void Molecule::applyNextFrame( const uint p_frameCount )
		{
			int		  newFrame	= _currentFrame;
			const int lastFrame = getFrameCount() - 1;

			const bool playForward = _playMode == Trajectory::PlayMode::Once || _playMode == Trajectory::PlayMode::Loop
									 || ( _playMode == Trajectory::PlayMode::PingPong && _dynamicLoopCount % 2 == 0 );

			if ( playForward )
			{
				newFrame += p_frameCount;

				if ( newFrame >= lastFrame )
				{
					_dynamicLoopCount += 1 + ( newFrame / ( getFrameCount() + 1 ) );
					if ( _playMode == Trajectory::PlayMode::Loop )
						newFrame = newFrame % getFrameCount();
					else
						newFrame = lastFrame;
				}
			}
			else
			{
				newFrame -= p_frameCount;

				if ( newFrame <= 0 )
				{
					_dynamicLoopCount += 1 + ( std::abs( newFrame ) / ( getFrameCount() + 1 ) );

					if ( _playMode == Trajectory::PlayMode::RevertLoop )
						newFrame = ( getFrameCount() + newFrame ) % getFrameCount();
					else
						newFrame = 0;
				}
			}

			setFrame( newFrame );

			const bool playFinished = ( _playMode == Trajectory::PlayMode::Once && _currentFrame == lastFrame )
									  || ( _playMode == Trajectory::PlayMode::RevertOnce && _currentFrame == 0 );

			if ( playFinished )
			{
				setIsPlaying( false );
			}
		}

		void Molecule::setFPS( const uint p_fps )
		{
			if ( _fps != p_fps )
			{
				_fps = p_fps;
				_notifyViews( new Event::VTXEvent( Event::Model::TRAJECTORY_DATA_CHANGE ) );
			}
		}
		void Molecule::setIsPlaying( const bool p_isPlaying )
		{
			if ( _isPlaying != p_isPlaying )
			{
				_isPlaying		  = p_isPlaying;
				_dynamicLoopCount = 0;
				_trajectoryTimer  = 0;

				_notifyViews( new Event::VTXEvent( Event::Model::TRAJECTORY_DATA_CHANGE ) );
			}
		}
		void Molecule::setPlayMode( const Trajectory::PlayMode & p_playMode )
		{
			if ( _playMode != p_playMode )
			{
				_playMode = p_playMode;
				_notifyViews( new Event::VTXEvent( Event::Model::TRAJECTORY_DATA_CHANGE ) );
			}
		}

		void Molecule::updateTrajectory( const float & p_deltaTime )
		{
			const uint frameCount = getFrameCount();

			if ( !hasTrajectory() || !isPlaying() || getPlayMode() == Trajectory::PlayMode::Stop )
				return;

			_trajectoryTimer += p_deltaTime;

			const uint fps = getFPS();

			if ( fps == 0u )
			{
				applyNextFrame();
			}
			else
			{
				const uint frame	 = getFrame();
				uint	   nextFrame = frame;

				float offset = 1.f / float( fps );
				while ( _trajectoryTimer >= offset )
				{
					nextFrame++;
					_trajectoryTimer -= offset;
				}

				if ( nextFrame != frame )
				{
					applyNextFrame( nextFrame - frame );
				}
			}
		}

		bool Molecule::isAtEndOfTrajectoryPlay()
		{
			bool res;

			switch ( _playMode )
			{
			case Trajectory::PlayMode::Once: res = _currentFrame >= getFrameCount() - 1; break;
			case Trajectory::PlayMode::RevertOnce: res = _currentFrame <= 0; break;
			case Trajectory::PlayMode::Loop:
			case Trajectory::PlayMode::RevertLoop:
			case Trajectory::PlayMode::PingPong:
			case Trajectory::PlayMode::Stop: res = false; break;
			default:
				VTX_WARNING( "PlayMode " + std::to_string( int( _playMode ) )
							 + "not managed in Molecule::isAtEndOfTrajectoryPlay." );
				res = false;
				break;
			}

			return res;
		}
		void Molecule::resetTrajectoryPlay()
		{
			int frame;
			switch ( _playMode )
			{
			case Trajectory::PlayMode::Loop:
			case Trajectory::PlayMode::PingPong:
			case Trajectory::PlayMode::Stop:
			case Trajectory::PlayMode::Once: frame = 0; break;
			case Trajectory::PlayMode::RevertOnce:
			case Trajectory::PlayMode::RevertLoop: frame = getFrameCount() - 1; break;
			default:
				VTX_WARNING( "PlayMode " + std::to_string( int( _playMode ) )
							 + "not managed in Molecule::resetTrajectoryPlay." );
				frame = 0;
				break;
			}

			_trajectoryTimer  = 0;
			_dynamicLoopCount = 0;
			setFrame( frame );
		}

		void Molecule::setShowWater( const bool p_showWater )
		{
			_showWater = p_showWater;
			_fillBufferAtomVisibilities();
			VTX_EVENT( new Event::VTXEvent( Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE ) );
		}
		void Molecule::setShowHydrogen( const bool p_showHydrogen )
		{
			_showHydrogen = p_showHydrogen;
			_fillBufferAtomVisibilities();
			VTX_EVENT( new Event::VTXEvent( Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE ) );
		}
		void Molecule::setShowSolvent( const bool p_showSolvent )
		{
			_showSolvent = p_showSolvent;
			_fillBufferAtomVisibilities();
			VTX_EVENT( new Event::VTXEvent( Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE ) );
		}
		void Molecule::setShowIon( const bool p_showIon )
		{
			_showIon = p_showIon;
			_fillBufferAtomVisibilities();
			VTX_EVENT( new Event::VTXEvent( Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE ) );
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

		void Molecule::render( const Object3D::Camera & p_camera ) const
		{
			BaseModel3D::render( p_camera );

			if ( _secondaryStructure != nullptr )
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
			/// TODO: this is strange
			_secondaryStructure->init( getBuffer()->getGL() );
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
