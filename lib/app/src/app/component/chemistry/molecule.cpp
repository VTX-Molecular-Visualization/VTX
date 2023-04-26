#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/solvent_excluded_surface.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/model/representation/representation_library.hpp"
#include "app/model/selection.hpp"
#include "app/mvc.hpp"
#include "app/old_app/color/rgba.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/representation/representation_manager.hpp"
#include "app/old_app/util/molecule.hpp"
#include "app/old_app/util/secondary_structure.hpp"
#include "app/old_app/view/d3/cylinder.hpp"
#include "app/old_app/view/d3/sphere.hpp"
#include "app/old_app/vtx_app.hpp"
#include <algorithm>
#include <util/logger.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::App::Internal::ChemDB;

	Molecule::Molecule() : Molecule( VTX::ID::Model::MODEL_MOLECULE ) {}
	Molecule::Molecule( const VTX::ID::VTX_ID & p_typeId ) : BaseModel3D( VTX::ID::Model::MODEL_MOLECULE )
	{
		_categories.resize( int( ChemDB::Category::TYPE::COUNT ) );

		for ( int i = 0; i < int( ChemDB::Category::TYPE::COUNT ); i++ )
		{
			_categories[ i ] = VTX::MVC_MANAGER().instantiateModel<Chemistry::Category>();
			_categories[ i ]->setMoleculePtr( this );
			_categories[ i ]->setCategoryEnum( ChemDB::Category::TYPE( i ) );
		}

		_playMode = VTX_SETTING().getDefaultTrajectoryPlayMode();
		_fps	  = VTX_SETTING().getDefaultTrajectorySpeed();
	}

	Molecule::~Molecule()
	{
		VTX::MVC_MANAGER().deleteAllModels( _atoms );
		VTX::MVC_MANAGER().deleteAllModels( _bonds );
		VTX::MVC_MANAGER().deleteAllModels( _residues );
		VTX::MVC_MANAGER().deleteAllModels( _chains );
		VTX::MVC_MANAGER().deleteAllModels( _categories );

		for ( const Internal::ChemDB::UnknownResidueData * const unknownResidueSymbol : _unknownResidueSymbol )
			delete unknownResidueSymbol;

		if ( _secondaryStructure != nullptr )
		{
			VTX::MVC_MANAGER().deleteModel( _secondaryStructure );
		}
		for ( auto const & [ key, val ] : _solventExcludedSurfaces )
		{
			VTX::MVC_MANAGER().deleteModel( val );
		}
	}

	void Molecule::setPdbIdCode( const std::string & p_pdbId ) { _pdbIdCode = p_pdbId; }

	Chain & Molecule::addChain()
	{
		Chain * const chain = VTX::MVC_MANAGER().instantiateModel<Chain>();
		_addChain( chain );
		return *chain;
	}
	void Molecule::_addChain( Chemistry::Chain * const p_chain )
	{
		_chains.emplace_back( p_chain );
		_realChainCount++;
	}

	Residue & Molecule::addResidue()
	{
		Residue * const residue = VTX::MVC_MANAGER().instantiateModel<Residue>();
		_residues.emplace_back( residue );
		return *residue;
	}

	Atom & Molecule::addAtom()
	{
		Atom * const atom = VTX::MVC_MANAGER().instantiateModel<Atom>();
		_atoms.emplace_back( atom );
		return *atom;
	}

	Bond & Molecule::addBond()
	{
		Bond * const bond = VTX::MVC_MANAGER().instantiateModel<Bond>();
		bond->setMoleculePtr( this );
		_bonds.emplace_back( bond );
		return *bond;
	}

	int Molecule::getUnknownResidueSymbolIndex( const std::string & p_symbol ) const
	{
		int residueIndex;

		for ( residueIndex = 0; residueIndex < _unknownResidueSymbol.size(); residueIndex++ )
		{
			if ( _unknownResidueSymbol[ residueIndex ]->symbolStr == p_symbol )
				return residueIndex;
		}

		return -1;
	}
	Internal::ChemDB::UnknownResidueData * const Molecule::getUnknownResidueSymbol(
		const uint p_unkownymbolIndex ) const
	{
		return _unknownResidueSymbol[ p_unkownymbolIndex ];
	}

	Internal::ChemDB::UnknownResidueData * const Molecule::getUnknownResidueSymbol( const std::string & p_symbol ) const
	{
		for ( int residueIndex = 0; residueIndex < _unknownResidueSymbol.size(); residueIndex++ )
		{
			if ( _unknownResidueSymbol[ residueIndex ]->symbolStr == p_symbol )
				return _unknownResidueSymbol[ residueIndex ];
		}

		return nullptr;
	}

	int Molecule::addUnknownResidueSymbol( Internal::ChemDB::UnknownResidueData * const p_residueData )
	{
		int residueIndex;

		for ( residueIndex = 0; residueIndex < _unknownResidueSymbol.size(); residueIndex++ )
		{
			if ( _unknownResidueSymbol[ residueIndex ]->symbolStr == p_residueData->symbolStr )
				return residueIndex;
		}

		_unknownResidueSymbol.emplace_back( p_residueData );
		return residueIndex;
	}

	void Molecule::_init()
	{
		BaseModel::setDefaultName( &_displayName );

		// Fill buffers.
		if ( _atomPositionsFrames.size() > 0 )
		{
			initBaseRepresentable( this, nullptr, this );

			if ( !hasCustomRepresentation() )
			{
				VTX::Representation::RepresentationManager::get().instantiateDefaultRepresentation(
					*this, false, false );

				_defaultRepresentationIDs.reserve( getCategories().size() );
				for ( Chemistry::Category * const category : getCategories() )
				{
					if ( category->isEmpty() )
						continue;

					const Model::Representation::Representation * const defaultRepresentation
						= VTXApp::get().getRepresentationLibrary().getDefaultRepresentation(
							category->getCategoryEnum() );

					for ( const uint chainIndex : category->getChains() )
					{
						Chemistry::Chain * const chain = getChain( chainIndex );

						if ( chain == nullptr )
							continue;

						const Model::Representation::InstantiatedRepresentation * const
							defaultInstantiatedRepresentation
							= VTX::Representation::RepresentationManager::get().instantiateRepresentation(
								defaultRepresentation, *chain, false, false );

						_markRepresentationAsDefault( defaultInstantiatedRepresentation );
					}
				}
				_defaultRepresentationIDs.shrink_to_fit();
			}

			_buffer->setAtomPositions( _atomPositionsFrames[ _currentFrame ], _atomPositionsFrames.size() > 1 );
			_buffer->setAtomRadius( _bufferAtomRadius );
			_buffer->setAtomVisibilities( _bufferAtomVisibilities );
			_buffer->setAtomSelections( _bufferAtomSelections );
			_buffer->setAtomIds( _bufferAtomIds );
			if ( _bufferBonds.empty() == false )
			{
				_buffer->setBonds( _bufferBonds );
			}

			computeAllRepresentationData();
			_fillBufferAtomColors();
		}
	}

	void Molecule::_markRepresentationAsDefault(
		const Model::Representation::InstantiatedRepresentation * const _instantiatedRepresentation )
	{
		_defaultRepresentationIDs.emplace_back( _instantiatedRepresentation->getId() );
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

	void Molecule::clearDefaultRepresentations()
	{
		if ( _defaultRepresentationIDs.size() <= 0 )
			return;

		for ( const App::Core::Model::ID & instantiatedRepresentationID : _defaultRepresentationIDs )
		{
			if ( !VTX::MVC_MANAGER().doesModelExists( instantiatedRepresentationID ) )
				continue;

			Model::Representation::InstantiatedRepresentation & instantiatedRepresentation
				= VTX::MVC_MANAGER().getModel<Model::Representation::InstantiatedRepresentation>(
					instantiatedRepresentationID );

			VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
				*instantiatedRepresentation.getTarget(), false, false );
		}

		_defaultRepresentationIDs.clear();
		_defaultRepresentationIDs.shrink_to_fit();
	}

	bool Molecule::isDefaultRepresentation(
		const Model::Representation::InstantiatedRepresentation & p_representation ) const
	{
		return std::find( _defaultRepresentationIDs.begin(), _defaultRepresentationIDs.end(), p_representation.getId() )
			   != _defaultRepresentationIDs.end();
	}

	void Molecule::removeChildrenRepresentations()
	{
		for ( Chemistry::Chain * const chain : _chains )
		{
			if ( chain == nullptr )
				continue;

			VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation( *chain, false, false );

			chain->removeChildrenRepresentations();
		}
	}

	void Molecule::_onRepresentationChange() { _notifyViews( App::Event::Model::REPRESENTATION_CHANGE ); }

	void Molecule::_computeAABB() const
	{
		_aabb.invalidate();

		for ( const Chemistry::Atom * const atom : _atoms )
		{
			if ( atom == nullptr )
				continue;

			// Compute AABB on all frames to ensure that all the trajectory may be visible with an orient
			const uint	atomIndex  = atom->getIndex();
			const float atomRadius = atom->getVdwRadius();
			for ( const AtomPositionsFrame & frame : _atomPositionsFrames )
			{
				_aabb.extend( frame[ atomIndex ], atomRadius );
			}
		}
	}

	void Molecule::_instantiate3DViews()
	{
		_addRenderable( VTX::MVC_MANAGER().instantiateView<View::D3::Sphere>( this, VTX::ID::View::D3_SPHERE ) );
		_addRenderable( VTX::MVC_MANAGER().instantiateView<View::D3::Cylinder>( this, VTX::ID::View::D3_CYLINDER ) );
	}

	void Molecule::resizeBuffers()
	{
		_bufferAtomRadius.resize( _atoms.size() );
		_bufferAtomVisibilities.resize( _atoms.size(), 1u );
		_bufferAtomColors.resize( _atoms.size(), Color::Rgba::WHITE );
		_bufferAtomSelections.resize( _atoms.size(), 0u );
		_bufferAtomIds.resize( _atoms.size() );
	}

	void Molecule::refreshBondsBuffer() { _buffer->setBonds( _bufferBonds ); }

	void Molecule::_fillBufferAtomColors()
	{
		_bufferAtomColors.resize( _atoms.size() );

		for ( const Chemistry::Residue * const residue : getResidues() )
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

			bool		colorCarbon = false;
			Color::Rgba color;

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

			case Generic::COLOR_MODE::INHERITED:
			default: break;
			}

			for ( uint i = residue->getIndexFirstAtom(); i < residue->getIndexFirstAtom() + residue->getAtomCount();
				  i++ )
			{
				const Chemistry::Atom * const atom = getAtom( i );

				if ( atom == nullptr )
					continue;

				if ( colorCarbon && atom->getSymbol() != ChemDB::Atom::SYMBOL::A_C )
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

			const bool displayHydrogen = showHydrogen();
			const bool displaySolvent  = showSolvent();
			const bool displayIons	   = showIon();

			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				const Atom * const atom = _atoms[ i ];

				if ( atom == nullptr )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				else if ( atom->getChainPtr()->isVisible() == false )
				{
					// Optimisation removed bacause some .gro file open by chemfiles may not have contiguous indexes
					// If the chain is not visible, we can directly fill all the atom in it
					// const uint			   firstAtomIndexInChain = i;
					// const Chemistry::Residue & lastResidueInChain
					//	= *getResidue( atom->getChainPtr()->getIndexLastResidue() );
					// const uint atomIndexInNextChain
					//	= lastResidueInChain.getIndexFirstAtom() + lastResidueInChain.getAtomCount();

					// const uint count = atomIndexInNextChain - firstAtomIndexInChain;

					// std::fill_n( _bufferAtomVisibilities.begin() + firstAtomIndexInChain, count, 0u );
					// i += count - 1;

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
				else if ( displaySolvent == false && atom->getType() == ChemDB::Atom::TYPE::SOLVENT )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				else if ( displayIons == false && atom->getType() == ChemDB::Atom::TYPE::ION )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				else if ( displayHydrogen == false && atom->getSymbol() == ChemDB::Atom::SYMBOL::A_H )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
			}
		}

		_buffer->setAtomVisibilities( _bufferAtomVisibilities );
	}

	void Molecule::_fillBufferAtomSelections( const Model::Selection::MapChainIds * const p_selection )
	{
		_bufferAtomSelections.clear();
		_bufferAtomSelections.resize( _atoms.size(), 0u );
		if ( p_selection != nullptr )
		{
			// Optimize buffer writing for full molecule
			if ( p_selection->getFullySelectedChildCount() == getRealChainCount() )
			{
				std::fill( _bufferAtomSelections.begin(), _bufferAtomSelections.end(), 1u );
			}
			else
			{
				for ( const Model::Selection::PairChainIds & pairChain : *p_selection )
				{
					// Optimize buffer writing for full chains
					const Chemistry::Chain * const chain = getChain( pairChain.first );
					if ( pairChain.second.getFullySelectedChildCount() == chain->getRealResidueCount() )
					{
						const uint firstAtomID = getResidue( chain->getIndexFirstResidue() )->getIndexFirstAtom();
						const Chemistry::Residue * const lastResidue = getResidue( chain->getIndexLastResidue() );
						const uint firstNextAtomID = lastResidue->getIndexFirstAtom() + lastResidue->getAtomCount();

						std::fill( _bufferAtomSelections.begin() + firstAtomID,
								   _bufferAtomSelections.begin() + firstNextAtomID,
								   1u );
					}
					else
					{
						// Optimization like previously will not works for residues because it will add too much
						// computation compared to add atoms one by one
						for ( const Model::Selection::PairResidueIds & pairResidue : pairChain.second )
						{
							for ( const uint & atomIndex : pairResidue.second )
							{
								_bufferAtomSelections[ atomIndex ] = 1u;
							}
						}
					}
				}
			}
		}

		_buffer->setAtomSelections( _bufferAtomSelections );
	}

	void Molecule::refreshStructure()
	{
		_buffer->setAtomVisibilities( _bufferAtomVisibilities );
		refreshBondsBuffer();

		// Refresh SS.
		refreshSecondaryStructure();

		// Delete SES, will be recomputed when needed.
		for ( auto & [ categoryEnum, sesCurrent ] : _solventExcludedSurfaces )
		{
			VTX::MVC_MANAGER().deleteModel( sesCurrent );
		}
		_solventExcludedSurfaces.clear();
	}

	void Molecule::refreshColors()
	{
		_fillBufferAtomColors();
		if ( _secondaryStructure != nullptr )
		{
			_secondaryStructure->refreshColors();
		}
		for ( auto const & [ key, val ] : _solventExcludedSurfaces )
		{
			val->refreshColors();
		}
	}

	void Molecule::refreshVisibilities()
	{
		_fillBufferAtomVisibilities();
		if ( _secondaryStructure != nullptr )
		{
			_secondaryStructure->refreshVisibilities();
		}
		for ( auto const & [ key, val ] : _solventExcludedSurfaces )
		{
			val->refreshVisibilities();
		}
	}

	void Molecule::refreshSelection( const Model::Selection::MapChainIds * const p_selection )
	{
		_fillBufferAtomSelections( p_selection );
		if ( _secondaryStructure != nullptr )
		{
			_secondaryStructure->refreshSelection( p_selection );
		}
		for ( auto const & [ key, val ] : _solventExcludedSurfaces )
		{
			val->refreshSelections();
		}
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
		if ( _buffer != nullptr )
		{
			_buffer->setAtomPositions( _atomPositionsFrames[ _currentFrame ], _atomPositionsFrames.size() > 1 );
		}
		if ( _secondaryStructure != nullptr )
			_secondaryStructure->refresh();

		refreshSolventExcludedSurfaces();
		refreshRepresentationTargets();

		_notifyViews( App::Event::Model::TRAJECTORY_FRAME_CHANGE );

		VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
	}

	void Molecule::applyNextFrame( const uint p_frameCount )
	{
		int		  newFrame	= _currentFrame;
		const int lastFrame = getFrameCount() - 1;

		const bool playForward
			= _playMode == Component::Chemistry::PlayMode::Once || _playMode == Component::Chemistry::PlayMode::Loop
			  || ( _playMode == Component::Chemistry::PlayMode::PingPong && _dynamicLoopCount % 2 == 0 );

		if ( playForward )
		{
			newFrame += p_frameCount;

			if ( newFrame >= lastFrame )
			{
				_dynamicLoopCount += 1 + ( newFrame / ( getFrameCount() + 1 ) );
				if ( _playMode == Component::Chemistry::PlayMode::Loop )
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

				if ( _playMode == Component::Chemistry::PlayMode::RevertLoop )
					newFrame = ( getFrameCount() + newFrame ) % getFrameCount();
				else
					newFrame = 0;
			}
		}

		setFrame( newFrame );

		const bool playFinished = ( _playMode == Component::Chemistry::PlayMode::Once && _currentFrame == lastFrame )
								  || ( _playMode == Component::Chemistry::PlayMode::RevertOnce && _currentFrame == 0 );

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
			_notifyViews( App::Event::Model::TRAJECTORY_DATA_CHANGE );
		}
	}
	void Molecule::setIsPlaying( const bool p_isPlaying )
	{
		if ( _isPlaying != p_isPlaying )
		{
			_isPlaying		  = p_isPlaying;
			_dynamicLoopCount = 0;
			_trajectoryTimer  = 0;

			_notifyViews( App::Event::Model::TRAJECTORY_DATA_CHANGE );
		}
	}
	void Molecule::setPlayMode( const Component::Chemistry::PlayMode & p_playMode )
	{
		if ( _playMode != p_playMode )
		{
			_playMode = p_playMode;
			_notifyViews( App::Event::Model::TRAJECTORY_DATA_CHANGE );
		}
	}

	void Molecule::updateTrajectory( const float & p_deltaTime )
	{
		if ( !hasTrajectory() || !isPlaying() || getPlayMode() == Component::Chemistry::PlayMode::Stop )
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
		case Component::Chemistry::PlayMode::Once: res = _currentFrame >= getFrameCount() - 1; break;
		case Component::Chemistry::PlayMode::RevertOnce: res = _currentFrame <= 0; break;
		case Component::Chemistry::PlayMode::Loop:
		case Component::Chemistry::PlayMode::RevertLoop:
		case Component::Chemistry::PlayMode::PingPong:
		case Component::Chemistry::PlayMode::Stop: res = false; break;
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
		case Component::Chemistry::PlayMode::Loop:
		case Component::Chemistry::PlayMode::PingPong:
		case Component::Chemistry::PlayMode::Stop:
		case Component::Chemistry::PlayMode::Once: frame = 0; break;
		case Component::Chemistry::PlayMode::RevertOnce:
		case Component::Chemistry::PlayMode::RevertLoop: frame = getFrameCount() - 1; break;
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
	void Molecule::forceNotifyTrajectoryChanged() { _notifyViews( App::Event::Model::TRAJECTORY_DATA_CHANGE ); }

	bool Molecule::showWater() const { return getCategory( ChemDB::Category::TYPE::WATER ).isVisible(); }
	void Molecule::setShowWater( const bool p_showWater )
	{
		Util::Molecule::show( getCategory( ChemDB::Category::TYPE::WATER ), p_showWater );
		_fillBufferAtomVisibilities();
		VTX_EVENT( VTX::App::Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE );
	}
	bool Molecule::showHydrogen() const { return _showHydrogen; }
	void Molecule::setShowHydrogen( const bool p_showHydrogen )
	{
		_showHydrogen = p_showHydrogen;
		_fillBufferAtomVisibilities();
		VTX_EVENT( VTX::App::Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE );
	}
	bool Molecule::showSolvent() const { return getCategory( ChemDB::Category::TYPE::SOLVENT ).isVisible(); }
	void Molecule::setShowSolvent( const bool p_showSolvent )
	{
		Util::Molecule::show( getCategory( ChemDB::Category::TYPE::SOLVENT ), p_showSolvent );
		_fillBufferAtomVisibilities();
		VTX_EVENT( VTX::App::Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE );
	}
	bool Molecule::showIon() const { return getCategory( ChemDB::Category::TYPE::ION ).isVisible(); }
	void Molecule::setShowIon( const bool p_showIon )
	{
		Util::Molecule::show( getCategory( ChemDB::Category::TYPE::ION ), p_showIon );
		_fillBufferAtomVisibilities();
		VTX_EVENT( VTX::App::Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE );
	}

	void Molecule::print() const
	{
		// TODO: add more infos in debug (solvents, ions, ss...).
		VTX_INFO( "Molecule: " + _name );
		VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: " + std::to_string( _residues.size() )
				  + " / Atoms: " + std::to_string( _atoms.size() ) + " / Bonds: " + std::to_string( _bonds.size() ) );

		// Display unknown symbols.
		const std::vector<ChemDB::UnknownResidueData *> & unknownResidueSymbols = getUnknownResidueSymbols();
		if ( unknownResidueSymbols.empty() == false )
		{
			std::string unknownResidueSymbolsStr = "";
			for ( const ChemDB::UnknownResidueData * const unknownResidueData : unknownResidueSymbols )
			{
				unknownResidueSymbolsStr += unknownResidueData->symbolStr + " ";
			}
			VTX_INFO( "Unknown residue symbols : " + unknownResidueSymbolsStr );
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
		{
			_secondaryStructure->render( p_camera );
		}
		for ( auto const & [ key, val ] : _solventExcludedSurfaces )
		{
			val->render( p_camera );
		}
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
		VTX::MVC_MANAGER().deleteAllModels( _chains );
		VTX::MVC_MANAGER().deleteAllModels( _residues );

		// Create models.
		_chains.resize( p_molecule.getChainCount() );
		for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
		{
			getChains()[ i ] = VTX::MVC_MANAGER().instantiateModel<Chain>();
		}
		_residues.resize( p_molecule.getResidueCount() );
		for ( uint i = 0; i < p_molecule.getResidueCount(); ++i )
		{
			getResidues()[ i ] = VTX::MVC_MANAGER().instantiateModel<Residue>();
		}

		setName( p_molecule.getName() );
		// Loop over other molecule chains.
		for ( uint chainIndex = 0; chainIndex < p_molecule.getChainCount(); ++chainIndex )
		{
			const Chain &	   otherChain = *p_molecule.getChain( chainIndex );
			Chemistry::Chain & modelChain = *getChain( chainIndex );
			modelChain.setIndex( otherChain.getIndex() );
			modelChain.setMoleculePtr( this );
			modelChain.setName( otherChain.getName() );
			modelChain.setIndexFirstResidue( otherChain.getIndexFirstResidue() );
			modelChain.setResidueCount( otherChain.getResidueCount() );

			for ( uint residueIndex = 0; residueIndex < otherChain.getResidueCount(); ++residueIndex )
			{
				const Residue * const otherResidue
					= p_molecule.getResidue( otherChain.getIndexFirstResidue() + residueIndex );
				Chemistry::Residue * const modelResidue
					= getResidue( otherChain.getIndexFirstResidue() + residueIndex );

				modelResidue->setIndex( otherResidue->getIndex() );
				modelResidue->setChainPtr( &modelChain );
				modelResidue->setIndexFirstAtom( otherResidue->getIndexFirstAtom() );
				modelResidue->setAtomCount( otherResidue->getAtomCount() );
				modelResidue->setSymbol( otherResidue->getSymbol() );

				for ( uint atomIndex = 0; atomIndex < otherResidue->getAtomCount(); ++atomIndex )
				{
					const Atom &	  otherAtom = *p_molecule.getAtom( otherResidue->getIndexFirstAtom() + atomIndex );
					Chemistry::Atom & modelAtom = *getAtom( otherResidue->getIndexFirstAtom() + atomIndex );
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
			Chemistry::Chain &	 modelChain	  = *getChain( getChainCount() - 1 );
			Chemistry::Residue & modelResidue = *getResidue( getResidueCount() - 1 );
			modelChain.setIndex( getChainCount() - 1 );
			modelChain.setIndexFirstResidue( getResidueCount() - 1 );
			modelChain.setResidueCount( 1 );
			modelResidue.setIndex( getResidueCount() - 1 );
			modelResidue.setChainPtr( &modelChain );
			modelResidue.setIndexFirstAtom( p_molecule.getAtomCount() );
			modelResidue.setAtomCount( getAtomCount() - p_molecule.getAtomCount() );

			for ( uint atomIndex = 0; atomIndex < modelResidue.getAtomCount(); ++atomIndex )
			{
				Chemistry::Atom & modelAtom = *getAtom( modelResidue.getIndexFirstAtom() + atomIndex );
				modelAtom.setResiduePtr( &modelResidue );
			}
		}

		return true;
	}

	void Molecule::createSecondaryStructure()
	{
		assert( _secondaryStructure == nullptr );

		// Compute secondary structure if not loaded.
		if ( _configuration.isSecondaryStructureLoadedFromFile == false )
		{
			Util::SecondaryStructure::computeSecondaryStructure( *this );
		}

		_secondaryStructure = VTX::MVC_MANAGER().instantiateModel<SecondaryStructure, Molecule>( this );
		_secondaryStructure->init();
	}

	void Molecule::refreshSecondaryStructure()
	{
		if ( _secondaryStructure == nullptr )
		{
			return;
		}

		_secondaryStructure->refresh();
	}

	bool Molecule::hasSolventExcludedSurface( const ChemDB::Category::TYPE & p_categoryEnum ) const
	{
		return _solventExcludedSurfaces.find( p_categoryEnum ) != _solventExcludedSurfaces.end();
	}

	SolventExcludedSurface & Molecule::getSolventExcludedSurface( const ChemDB::Category::TYPE & p_categoryEnum )
	{
		assert( hasSolventExcludedSurface( p_categoryEnum ) );

		return *_solventExcludedSurfaces[ p_categoryEnum ];
	}

	void Molecule::createSolventExcludedSurface( const ChemDB::Category::TYPE & p_categoryEnum )
	{
		assert( hasSolventExcludedSurface( p_categoryEnum ) == false );
		assert( getCategory( p_categoryEnum ).isEmpty() == false );

		SolventExcludedSurface * const ses
			= VTX::MVC_MANAGER().instantiateModel<SolventExcludedSurface, Category>( &getCategory( p_categoryEnum ) );
		_solventExcludedSurfaces.emplace( p_categoryEnum, ses );
		ses->init();
	}

	void Molecule::refreshSolventExcludedSurfaces()
	{
		for ( auto & [ categoryEnum, sesCurrent ] : _solventExcludedSurfaces )
		{
			VTX::MVC_MANAGER().deleteModel( sesCurrent );

			SolventExcludedSurface * const ses
				= VTX::MVC_MANAGER().instantiateModel<SolventExcludedSurface, Category>( &getCategory( categoryEnum ) );

			sesCurrent = ses;
			sesCurrent->init();
		}
	}

	std::vector<Chemistry::Category *> Molecule::getFilledCategories() const
	{
		std::vector<Chemistry::Category *> res = std::vector<Chemistry::Category *>();
		res.reserve( int( ChemDB::Category::TYPE::COUNT ) );

		for ( Chemistry::Category * const category : _categories )
		{
			if ( !category->isEmpty() )
				res.emplace_back( category );
		}

		return res;
	};
	Chemistry::Category * Molecule::getCategoryFromChain( const Chemistry::Chain & p_chain )
	{
		return _categories[ int( p_chain.getCategoryEnum() ) ];
	}
	const Chemistry::Category * Molecule::getCategoryFromChain( const Chemistry::Chain & p_chain ) const
	{
		return _categories[ int( p_chain.getCategoryEnum() ) ];
	}

	void Molecule::setVisible( const bool p_visible )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			_notifyViews( App::Event::Model::MOLECULE_VISIBILITY );
		}
	}
	void Molecule::setVisible( const bool p_visible, const bool p_notify )
	{
		const bool previousVisibleState = isVisible();

		BaseVisible::setVisible( p_visible );

		if ( previousVisibleState != p_visible )
		{
			if ( p_notify )
				_notifyViews( App::Event::Model::MOLECULE_VISIBILITY );
		}
	}

	Chain * Molecule::getFirstChain()
	{
		for ( Chemistry::Chain * const chain : _chains )
			if ( chain != nullptr )
				return chain;

		return nullptr;
	}
	const Chain * const Molecule::getFirstChain() const
	{
		for ( const Chemistry::Chain * const chain : _chains )
			if ( chain != nullptr )
				return chain;

		return nullptr;
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
	int Molecule::getRealResidueCount() const
	{
		int res = 0;

		for ( int chainIdx = 0; chainIdx < _chains.size(); chainIdx++ )
		{
			const Chemistry::Chain * const chain = _chains[ chainIdx ];
			if ( chain == nullptr )
				continue;
			res += chain->getRealResidueCount();
		}

		return res;
	}
	int Molecule::getRealAtomCount() const
	{
		int res = 0;

		for ( int residueIdx = 0; residueIdx < _residues.size(); residueIdx++ )
		{
			const Chemistry::Residue * const residue = _residues[ residueIdx ];
			if ( residue == nullptr )
				continue;
			res += residue->getRealAtomCount();
		}

		return res;
	}

	void Molecule::removeChain( const uint p_id, const bool p_delete, const bool p_recursive, const bool p_notifyViews )
	{
		if ( p_recursive )
		{
			// Delete Residues
			for ( uint residueIndex = _chains[ p_id ]->getIndexFirstResidue();
				  residueIndex < ( _chains[ p_id ]->getIndexFirstResidue() + _chains[ p_id ]->getResidueCount() );
				  residueIndex++ )
			{
				if ( _residues[ residueIndex ] != nullptr )
					removeResidue( residueIndex, p_delete, p_recursive, false, false );
			}
		}

		Chemistry::Chain * const chainToDelete = _chains[ p_id ];

		_chains[ p_id ] = nullptr;
		_realChainCount--;

		_invalidateAABB();

		// Notify
		if ( p_notifyViews )
			notifyStructureChange();

		VTX_EVENT<Chemistry::Chain *>( VTX::App::Event::Global::CHAIN_REMOVED, chainToDelete );

		// Delete chain
		if ( p_delete )
			VTX::MVC_MANAGER().deleteModel( chainToDelete );
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
		Chemistry::Residue * const residueToDelete = _residues[ p_id ];
		Chemistry::Chain * const   parent		   = residueToDelete->getChainPtr();

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
			_invalidateAABB();

			// Notify
			if ( p_notifyViews )
				notifyStructureChange();
		}

		VTX_EVENT<Chemistry::Residue *>( VTX::App::Event::Global::RESIDUE_REMOVED, residueToDelete );

		// Remove Residue
		if ( p_delete )
			VTX::MVC_MANAGER().deleteModel( residueToDelete );
	}

	void Molecule::removeAtom( const uint p_id,
							   const bool p_delete,
							   const bool p_checkInternalBonds,
							   const bool p_checkParentUpdate,
							   const bool p_notifyViews )
	{
		Chemistry::Residue * const parent = _atoms[ p_id ]->getResiduePtr();

		// Delete Bonds
		if ( p_checkInternalBonds )
		{
			for ( uint bondIndex = parent->getIndexFirstBond();
				  bondIndex < parent->getIndexFirstBond() + parent->getBondCount();
				  bondIndex++ )
			{
				const Bond * const bond = _bonds[ bondIndex ];

				if ( bond != nullptr )
				{
					const uint bondFirstAtomIndex  = bond->getIndexFirstAtom();
					const uint bondSecondAtomIndex = bond->getIndexSecondAtom();

					if ( bondFirstAtomIndex == p_id || bondSecondAtomIndex == p_id )
					{
						const Chemistry::Atom * const bondFirstAtom	 = _atoms[ bondFirstAtomIndex ];
						const Chemistry::Atom * const bondSecondAtom = _atoms[ bondSecondAtomIndex ];

						const Chemistry::Residue * const residueFirstAtomOfBond
							= bondFirstAtom == nullptr ? nullptr : bondFirstAtom->getResiduePtr();

						const Chemistry::Residue * const residueSecondAtomOfBond
							= bondSecondAtom == nullptr ? nullptr : bondSecondAtom->getResiduePtr();

						// If external bond => need to remove the bond of the other residue
						if ( residueFirstAtomOfBond != nullptr && residueSecondAtomOfBond != nullptr
							 && residueFirstAtomOfBond != residueSecondAtomOfBond )
						{
							if ( residueFirstAtomOfBond == parent )
							{
								const uint otherBondIndex
									= residueSecondAtomOfBond->findBondIndex( bondFirstAtomIndex, bondSecondAtomIndex );

								removeBond( bondIndex, p_delete, false );
								removeBond( otherBondIndex, p_delete, false );
							}
							else
							{
								const uint otherBondIndex
									= residueFirstAtomOfBond->findBondIndex( bondFirstAtomIndex, bondSecondAtomIndex );

								removeBond( bondIndex, p_delete, false );
								removeBond( otherBondIndex, p_delete, false );
							}
						}
						else
						{
							removeBond( bondIndex, p_delete, false );
						}
					}
				}
			}
		}

		Chemistry::Atom * const atomToDelete = _atoms[ p_id ];

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
			_invalidateAABB();

			// Notify
			if ( p_notifyViews )
				notifyStructureChange();
		}

		VTX_EVENT<Chemistry::Atom *>( VTX::App::Event::Global::ATOM_REMOVED, atomToDelete );

		// Delete Atom
		if ( p_delete )
			VTX::MVC_MANAGER().deleteModel( atomToDelete );
	}

	void Molecule::removeBond( const uint p_id, const bool p_delete, const bool p_notifyViews )
	{
		if ( p_delete )
			VTX::MVC_MANAGER().deleteModel( _bonds[ p_id ] );

		_bonds[ p_id ] = nullptr;

		_bufferBonds[ p_id * 2u ]	   = 0;
		_bufferBonds[ p_id * 2u + 1u ] = 0;

		if ( p_notifyViews )
			notifyStructureChange();
	}

	void Molecule::notifyStructureChange()
	{
		_notifyDataChanged();
		VTX_EVENT<Chemistry::Molecule *>( VTX::App::Event::Global::MOLECULE_STRUCTURE_CHANGE, this );
	}

	void Molecule::notifyVisibilityChange()
	{
		_notifyViews( new App::Core::Event::VTXEvent( App::Event::Model::VISIBILITY ) );
	}

	void Molecule::setDisplayName( const std::string & p_name )
	{
		_displayName = p_name;
		_notifyViews( App::Event::Model::DISPLAY_NAME_CHANGE );
	}

	void Molecule::setColor( const Color::Rgba & p_color )
	{
		Generic::BaseColorable::setColor( p_color );

		if ( isInit() )
		{
			_notifyViews( App::Event::Model::COLOR_CHANGE );
			VTX_EVENT<const Color::Rgba &>( VTX::App::Event::Global::MOLECULE_COLOR_CHANGE, p_color );
		}
	}
} // namespace VTX::App::Component::Chemistry
