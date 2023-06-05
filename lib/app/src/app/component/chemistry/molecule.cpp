#include "app/component/chemistry/molecule.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/application/selection/selection.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/solvent_excluded_surface.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/id.hpp"
#include "app/mvc.hpp"
#include "app/render/view/cylinder.hpp"
#include "app/render/view/sphere.hpp"
#include "app/util/molecule.hpp"
#include "app/util/secondary_structure.hpp"
#include "app/vtx_app.hpp"
#include <algorithm>
#include <core/chemdb/color.hpp>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	Molecule::Molecule() : Molecule( App::ID::Model::MODEL_MOLECULE ) {}
	Molecule::Molecule( const VTX::App::VTX_ID & p_typeId ) : BaseModel3D( App::ID::Model::MODEL_MOLECULE )
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
		delete _moleculeStruct;

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
		_moleculeStruct->addChain();

		return *chain;
	}

	Residue & Molecule::addResidue()
	{
		Residue * const residue = VTX::MVC_MANAGER().instantiateModel<Residue>();
		_moleculeStruct->addResidue();

		return *residue;
	}

	Atom & Molecule::addAtom()
	{
		Atom * const atom = VTX::MVC_MANAGER().instantiateModel<Atom>();
		_moleculeStruct->addAtom();

		return *atom;
	}

	Bond & Molecule::addBond()
	{
		Bond * const			  bond	  = VTX::MVC_MANAGER().instantiateModel<Bond>();
		VTX::Core::Struct::Bond & newBond = _moleculeStruct->addBond();
		newBond.setMoleculePtr( _moleculeStruct );

		return *bond;
	}

	void Molecule::_init()
	{
		BaseModel::setDefaultName( &_displayName );

		// Fill buffers.
		if ( getFrameCount() > 0 )
		{
			initBaseRepresentable( this, nullptr, this );

			if ( !hasCustomRepresentation() )
			{
				App::Application::Representation::RepresentationManager::get().instantiateDefaultRepresentation(
					*this, false, false );

				_defaultRepresentationIDs.reserve( getCategories().size() );
				for ( Chemistry::Category * const category : getCategories() )
				{
					if ( category->isEmpty() )
						continue;

					const App::Application::Representation::RepresentationPreset * const defaultRepresentation
						= App::VTXApp::get().getRepresentationLibrary().getDefaultRepresentation(
							category->getCategoryEnum() );

					for ( const uint chainIndex : category->getChains() )
					{
						Chemistry::Chain * const chain = getChain( chainIndex );

						if ( chain == nullptr )
							continue;

						const App::Application::Representation::InstantiatedRepresentation * const
							defaultInstantiatedRepresentation
							= App::Application::Representation::RepresentationManager::get().instantiateRepresentation(
								defaultRepresentation, *chain, false, false );

						_markRepresentationAsDefault( defaultInstantiatedRepresentation );
					}
				}
				_defaultRepresentationIDs.shrink_to_fit();
			}

			_buffer->setAtomPositions( getCurrentAtomPositionFrame(), hasTrajectory() );
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
		const App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation )
	{
		_defaultRepresentationIDs.emplace_back( _instantiatedRepresentation->getId() );
	}

	void Molecule::clearDefaultRepresentations()
	{
		if ( _defaultRepresentationIDs.size() <= 0 )
			return;

		for ( const App::Core::Model::ID & instantiatedRepresentationID : _defaultRepresentationIDs )
		{
			if ( !VTX::MVC_MANAGER().doesModelExists( instantiatedRepresentationID ) )
				continue;

			App::Application::Representation::InstantiatedRepresentation & instantiatedRepresentation
				= VTX::MVC_MANAGER().getModel<App::Application::Representation::InstantiatedRepresentation>(
					instantiatedRepresentationID );

			App::Application::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
				*instantiatedRepresentation.getTarget(), false, false );
		}

		_defaultRepresentationIDs.clear();
		_defaultRepresentationIDs.shrink_to_fit();
	}

	bool Molecule::isDefaultRepresentation(
		const App::Application::Representation::InstantiatedRepresentation & p_representation ) const
	{
		return std::find( _defaultRepresentationIDs.begin(), _defaultRepresentationIDs.end(), p_representation.getId() )
			   != _defaultRepresentationIDs.end();
	}

	void Molecule::removeChildrenRepresentations()
	{
		for ( Chemistry::Chain * const chain : getChains() )
		{
			if ( chain == nullptr )
				continue;

			App::Application::Representation::RepresentationManager::get().removeInstantiatedRepresentation(
				*chain, false, false );

			chain->removeChildrenRepresentations();
		}
	}

	void Molecule::_onRepresentationChange() { _notifyViews( App::Event::Model::REPRESENTATION_CHANGE ); }

	void Molecule::_computeAABB() const
	{
		_aabb.invalidate();

		for ( const Chemistry::Atom * const atom : getAtoms() )
		{
			if ( atom == nullptr )
				continue;

			// Compute AABB on all frames to ensure that all the trajectory may be visible with an orient
			const uint	atomIndex  = atom->getIndex();
			const float atomRadius = atom->getVdwRadius();
			for ( const AtomPositionsFrame & frame : getAtomPositionFrames() )
			{
				_aabb.extend( frame[ atomIndex ], atomRadius );
			}
		}
	}

	void Molecule::_instantiate3DViews()
	{
		_addRenderable(
			VTX::MVC_MANAGER().instantiateView<App::Render::View::Sphere>( this, VTX::App::ID::View::D3_SPHERE ) );
		_addRenderable(
			VTX::MVC_MANAGER().instantiateView<App::Render::View::Cylinder>( this, VTX::App::ID::View::D3_CYLINDER ) );
	}

	void Molecule::resizeBuffers()
	{
		_bufferAtomRadius.resize( getAtomCount() );
		_bufferAtomVisibilities.resize( getAtomCount(), 1u );
		_bufferAtomColors.resize( getAtomCount(), Util::Color::Rgba::WHITE );
		_bufferAtomSelections.resize( getAtomCount(), 0u );
		_bufferAtomIds.resize( getAtomCount() );
	}

	void Molecule::refreshBondsBuffer() { _buffer->setBonds( _bufferBonds ); }

	void Molecule::_fillBufferAtomColors()
	{
		_bufferAtomColors.resize( getAtomCount() );

		for ( const Chemistry::Residue * const residue : getResidues() )
		{
			// Skip hidden items.
			if ( residue == nullptr )
				continue;

			const App::Application::Representation::InstantiatedRepresentation * const currentRepresentation
				= residue->getRepresentation();

			VTX::Core::ChemDB::Color::COLOR_MODE colorMode = currentRepresentation->getColorMode();

			if ( colorMode == VTX::Core::ChemDB::Color::COLOR_MODE::INHERITED )
			{
				const VTX::Core::ChemDB::Color::COLOR_MODE & chainColorMode
					= residue->getChainPtr()->getRepresentation()->getColorMode();
				if ( chainColorMode != VTX::Core::ChemDB::Color::COLOR_MODE::INHERITED )
				{
					colorMode = chainColorMode;
				}
				else
				{
					const VTX::Core::ChemDB::Color::COLOR_MODE & moleculeColorMode
						= getRepresentation()->getColorMode();
					if ( moleculeColorMode != VTX::Core::ChemDB::Color::COLOR_MODE::INHERITED )
						colorMode = moleculeColorMode;
					else
						colorMode = VTX::App::Application::Setting::COLOR_MODE_DEFAULT;
				}
			}

			bool			  colorCarbon = false;
			Util::Color::Rgba color;

			switch ( colorMode )
			{
			case VTX::Core::ChemDB::Color::COLOR_MODE::ATOM_CHAIN: colorCarbon = true; [[fallthrough]];
			case VTX::Core::ChemDB::Color::COLOR_MODE::CHAIN: color = residue->getChainPtr()->getColor(); break;

			case VTX::Core::ChemDB::Color::COLOR_MODE::ATOM_PROTEIN: colorCarbon = true; [[fallthrough]];
			case VTX::Core::ChemDB::Color::COLOR_MODE::PROTEIN: color = getColor(); break;

			case VTX::Core::ChemDB::Color::COLOR_MODE::ATOM_CUSTOM: colorCarbon = true; [[fallthrough]];
			case VTX::Core::ChemDB::Color::COLOR_MODE::CUSTOM: color = currentRepresentation->getColor(); break;

			case VTX::Core::ChemDB::Color::COLOR_MODE::RESIDUE:
				colorCarbon = false;
				color		= residue->getColor();
				break;

			case VTX::Core::ChemDB::Color::COLOR_MODE::INHERITED:
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
			_bufferAtomVisibilities.resize( getAtomCount(), 0u );
		}
		else
		{
			_bufferAtomVisibilities.resize( getAtomCount(), 1u );

			const bool displayHydrogen = showHydrogen();
			const bool displaySolvent  = showSolvent();
			const bool displayIons	   = showIon();

			for ( uint i = 0; i < uint( getAtomCount() ); ++i )
			{
				const Atom * const atom = getAtom( i );

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

	void Molecule::_fillBufferAtomSelections(
		const App::Application::Selection::SelectionModel::MapChainIds * const p_selection )
	{
		_bufferAtomSelections.clear();
		_bufferAtomSelections.resize( getAtomCount(), 0u );
		if ( p_selection != nullptr )
		{
			// Optimize buffer writing for full molecule
			if ( p_selection->getFullySelectedChildCount() == getRealChainCount() )
			{
				std::fill( _bufferAtomSelections.begin(), _bufferAtomSelections.end(), 1u );
			}
			else
			{
				for ( const App::Application::Selection::SelectionModel::PairChainIds & pairChain : *p_selection )
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
						for ( const App::Application::Selection::SelectionModel::PairResidueIds & pairResidue :
							  pairChain.second )
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

	void Molecule::refreshSelection(
		const App::Application::Selection::SelectionModel::MapChainIds * const p_selection )
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
			VTX_WARNING( "Frame {}  does not exists / Count: {}.", p_frameIdx, getFrameCount() );
			return;
		}

		_currentFrame = p_frameIdx;
		if ( _buffer != nullptr )
		{
			_buffer->setAtomPositions( getCurrentAtomPositionFrame(), getFrameCount() > 1 );
		}
		if ( _secondaryStructure != nullptr )
			_secondaryStructure->refresh();

		refreshSolventExcludedSurfaces();
		refreshRepresentationTargets();

		_notifyViews( App::Event::Model::TRAJECTORY_FRAME_CHANGE );

		App::VTXApp::get().MASK |= App::Render::VTX_MASK_3D_MODEL_UPDATED;
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
			VTX_WARNING( "PlayMode {} not managed in Molecule::isAtEndOfTrajectoryPlay.", int( _playMode ) );
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
			VTX_WARNING( "PlayMode {} not managed in Molecule::resetTrajectoryPlay.", int( _playMode ) );
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
		Util::App::Molecule::show( getCategory( ChemDB::Category::TYPE::WATER ), p_showWater );
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
		Util::App::Molecule::show( getCategory( ChemDB::Category::TYPE::SOLVENT ), p_showSolvent );
		_fillBufferAtomVisibilities();
		VTX_EVENT( VTX::App::Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE );
	}
	bool Molecule::showIon() const { return getCategory( ChemDB::Category::TYPE::ION ).isVisible(); }
	void Molecule::setShowIon( const bool p_showIon )
	{
		Util::App::Molecule::show( getCategory( ChemDB::Category::TYPE::ION ), p_showIon );
		_fillBufferAtomVisibilities();
		VTX_EVENT( VTX::App::Event::Global::MOLECULE_ELEMENT_DISPLAY_CHANGE );
	}

	void Molecule::print() const
	{
		// TODO: add more infos in debug (solvents, ions, ss...).
		VTX_INFO( "Molecule: {}.", getName() );
		VTX_INFO( "Chains: {} / Residues: {} / Atoms: {} / Bonds: {}.",
				  getChainCount(),
				  getResidueCount(),
				  getAtomCount(),
				  getBondCount() );

		// Display unknown symbols.
		const std::vector<ChemDB::UnknownResidueData *> & unknownResidueSymbols = getUnknownResidueSymbols();
		if ( unknownResidueSymbols.empty() == false )
		{
			std::string unknownResidueSymbolsStr = "";
			for ( const ChemDB::UnknownResidueData * const unknownResidueData : unknownResidueSymbols )
			{
				unknownResidueSymbolsStr += unknownResidueData->symbolStr + " ";
			}
			VTX_INFO( "Unknown residue symbols : {}.", unknownResidueSymbolsStr );
		}

		const std::unordered_set<std::string> & unknownAtomSymbols = getUnknownAtomSymbols();
		if ( unknownAtomSymbols.empty() == false )
		{
			std::string unknownAtomSymbolsStr = "";
			for ( std::string symbol : unknownAtomSymbols )
			{
				unknownAtomSymbolsStr += symbol + " ";
			}
			VTX_WARNING( "Unknown atom symbols : {}.", unknownAtomSymbolsStr );
		}

		VTX_DEBUG( "Sizeof molecule: {}", sizeof( *this ) );
		VTX_DEBUG( "Sizeof chain: {}", sizeof( *getChain( 0 ) ) );
		VTX_DEBUG( "Sizeof residue: {}", sizeof( *getResidue( 0 ) ) );
		VTX_DEBUG( "Sizeof atom: {}", sizeof( *getAtom( 0 ) ) );
		VTX_DEBUG( "Sizeof bond: {}", sizeof( *getBond( 0 ) ) );
	}

	void Molecule::render( const App::Component::Render::Camera & p_camera ) const
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

	void Molecule::createSecondaryStructure()
	{
		assert( _secondaryStructure == nullptr );

		// Compute secondary structure if not loaded.
		if ( _configuration.isSecondaryStructureLoadedFromFile == false )
		{
			Util::App::SecondaryStructure::computeSecondaryStructure( *this );
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

	void Molecule::removeChain( const uint p_id, const bool p_delete, const bool p_recursive, const bool p_notifyViews )
	{
		Chemistry::Chain * const chainToDelete = getChain( p_id );

		_moleculeStruct->removeChain( p_id, p_delete, p_recursive );

		_invalidateAABB();

		// Notify
		if ( p_notifyViews )
			notifyStructureChange();

		VTX_EVENT<Chemistry::Chain *>( VTX::App::Event::Global::CHAIN_REMOVED, chainToDelete );

		if ( p_delete )
			VTX::MVC_MANAGER().deleteModel( chainToDelete );
	}

	void Molecule::removeResidue( const uint p_id,
								  const bool p_delete,
								  const bool p_recursive,
								  const bool p_checkParentUpdate,
								  const bool p_notifyViews )
	{
		Chemistry::Residue * const residueToDelete = getResidue( p_id );

		_moleculeStruct->removeResidue( p_id, p_delete, p_recursive, p_checkParentUpdate );

		_invalidateAABB();

		// Notify
		if ( p_notifyViews )
			notifyStructureChange();

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
		Chemistry::Atom * const atomToDelete = getAtom( p_id );

		_moleculeStruct->removeAtom( p_id, p_delete, p_checkInternalBonds, p_checkParentUpdate );

		_bufferAtomVisibilities[ p_id ] = 0u;
		_invalidateAABB();

		// Notify
		if ( p_notifyViews )
			notifyStructureChange();

		VTX_EVENT<Chemistry::Atom *>( VTX::App::Event::Global::ATOM_REMOVED, atomToDelete );

		// Delete Atom
		if ( p_delete )
			VTX::MVC_MANAGER().deleteModel( atomToDelete );
	}

	void Molecule::removeBond( const uint p_id, const bool p_delete, const bool p_notifyViews )
	{
		Chemistry::Bond * const bondToDelete = getBond( p_id );

		_moleculeStruct->removeBond( p_id, p_delete );

		_bufferBonds[ p_id * 2u ]	   = 0;
		_bufferBonds[ p_id * 2u + 1u ] = 0;

		if ( p_delete )
			VTX::MVC_MANAGER().deleteModel( bondToDelete );

		if ( p_notifyViews )
			notifyStructureChange();
	}

	void Molecule::notifyStructureChange()
	{
		_notifyDataChanged();
		VTX_EVENT<Chemistry::Molecule *>( Event::Global::MOLECULE_STRUCTURE_CHANGE, this );
	}

	void Molecule::notifyVisibilityChange() { _notifyViews( new Core::Event::VTXEvent( Event::Model::VISIBILITY ) ); }

	void Molecule::setDisplayName( const std::string & p_name )
	{
		_displayName = p_name;
		_notifyViews( Event::Model::DISPLAY_NAME_CHANGE );
	}

	void Molecule::setColor( const Util::Color::Rgba & p_color )
	{
		Component::Generic::BaseColorable::setColor( p_color );

		if ( isInit() )
		{
			_notifyViews( Event::Model::COLOR_CHANGE );
			VTX_EVENT<const Util::Color::Rgba &>( Event::Global::MOLECULE_COLOR_CHANGE, p_color );
		}
	}
} // namespace VTX::App::Component::Chemistry
